
#include <algorithm>
#include <vector>

#include "Sprites.h"
#include "D3D.h"
#include "Shaders/Shader_Sprites.h"
#include "Configuration.h"


namespace Pimp
{
	const unsigned int kMaxSprites = 4096;

	Sprites::Sprites() :
		effect((unsigned char*)gCompiledShader_Sprites, sizeof(gCompiledShader_Sprites)),
		effectTechnique(&effect, "Sprites"),
		effectPass(&effectTechnique, "Default")
	{
		// Glow's business.
		int varIndexRenderScale = effect.RegisterVariable("renderScale", true);
		const Vector2& visible_area = gD3D->GetRenderScale();
		effect.SetVariableValue(varIndexRenderScale, visible_area);
		varIndexTextureMap = effect.RegisterVariable("textureMap", true);

		// Plek's business.
		VB.vertices = gD3D->CreateVertexBuffer(6*kMaxSprites*sizeof(SpriteVertex), nullptr, true);
		sprites.clear();

		unsigned char* signature;
		int signatureLength;
		effectPass.GetVSInputSignature(&signature, &signatureLength);

		/* static const */ D3D10_INPUT_ELEMENT_DESC elemDesc[3];
		elemDesc[0].SemanticName = "POSITION";
		elemDesc[0].SemanticIndex = 0;
		elemDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		elemDesc[0].InputSlot = 0;
		elemDesc[0].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
		elemDesc[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		elemDesc[0].InstanceDataStepRate = 0;
		elemDesc[1].SemanticName = "COLOR";
		elemDesc[1].SemanticIndex = 0;
		elemDesc[1].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		elemDesc[1].InputSlot = 0;
		elemDesc[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
		elemDesc[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		elemDesc[1].InstanceDataStepRate = 0;
		elemDesc[2].SemanticName = "TEXCOORD";
		elemDesc[2].SemanticIndex = 0;
		elemDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		elemDesc[2].InputSlot = 0;
		elemDesc[2].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
		elemDesc[2].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		elemDesc[2].InstanceDataStepRate = 0;
		VB.inputLayout = gD3D->CreateInputLayout(elemDesc, 3, signature, signatureLength);

		delete [] signature;

	}

	Sprites::~Sprites()
	{
		// Member VB takes care of it's resources.
	}

	inline const Vector3 Rotate(const Vector2 &position, const Vector2 &pivot, float angle)
	{
		if (0.f == angle)
		{
			return Vector3(position.x, position.y, 0.f);
		}
		else
		{
			const Matrix4 mRotZ = CreateMatrixRotationZ(angle);
			return mRotZ.TransformCoord(Vector3(position.x-pivot.x, position.y-pivot.y, 1.f));
		}
	}

	void Sprites::AddSprite(
			Texture2D *pTexture,
			D3D::BlendMode blendMode,
			const unsigned int vertexColor,
			const Vector2 &topLeft,
			const Vector2 &size,
			float sortZ,
			float rotateZ /* = 0.f */)
	{
		ASSERT(blendMode < D3D::BlendMode::MAX_BlendMode);
		ASSERT(sprites.size() < kMaxSprites);

		// first sprite?
		if (sprites.empty())
		{
			// yes: lock vertex buffer
			VERIFY(SUCCEEDED(VB.vertices->Map(D3D10_MAP_WRITE_DISCARD, 0, reinterpret_cast<void **>(&pVertices))));
		}

		// hack: transform from top-left aligned 1920*1080 to homogenous space
		const Vector2 adjTopLeft = Vector2(-1.f + topLeft.x/1920.f*2.f, 1.f - topLeft.y/1080.f*2.f);
		const Vector2 adjSize = Vector2(size.x/1920.f*2.f, -size.y/1080.f*2.f);
		const Vector2 bottomRight = adjTopLeft + adjSize;
		const Vector2 quadPivot(adjTopLeft.x + adjSize.x*0.5f, adjTopLeft.y + adjSize.y*0.5f);
		const unsigned int ARGB = vertexColor;

		// triangle 1: bottom right
		pVertices->position = Rotate(Vector2(bottomRight.x, bottomRight.y), quadPivot, rotateZ);
		pVertices->ARGB = ARGB;
		pVertices->UV = Vector2(1.f, 1.f);
		++pVertices;
		// triangle 1: bottom left
		pVertices->position = Rotate(Vector2(adjTopLeft.x, bottomRight.y), quadPivot, rotateZ);
		pVertices->ARGB = ARGB;
		pVertices->UV = Vector2(0.f, 1.f);
		++pVertices;
		// triangle 1: top left
		pVertices->position = Rotate(Vector2(adjTopLeft.x, adjTopLeft.y), quadPivot, rotateZ);
		pVertices->ARGB = ARGB;
		pVertices->UV = Vector2(0.f, 0.f);
		++pVertices;
		// triangle 2: bottom right
		pVertices->position = Rotate(Vector2(bottomRight.x, bottomRight.y), quadPivot, rotateZ);
		pVertices->ARGB = ARGB;
		pVertices->UV = Vector2(1.f, 1.f);
		++pVertices;
		// triangle 2: top left
		pVertices->position = Rotate(Vector2(adjTopLeft.x, adjTopLeft.y), quadPivot, rotateZ);
		pVertices->ARGB = ARGB;
		pVertices->UV = Vector2(0.f, 0.f);
		++pVertices;
		// triangle 2: top right
		pVertices->position = Rotate(Vector2(bottomRight.x, adjTopLeft.y), quadPivot, rotateZ);
		pVertices->ARGB = ARGB;
		pVertices->UV = Vector2(1.f, 0.f);
		++pVertices;

		// add to list
		Sprite sprite;
		ASSERT(nullptr != pTexture);
		sprite.pTexture = pTexture;
		sprite.blendMode = blendMode;
		sprite.size = std::move(size);
		sprite.sortZ = sortZ;
		sprite.vertexOffs = sprites.size()*6;
		sprites.push_back(sprite);
	}

	void Sprites::Flush()
	{
		const float renderAspect = Configuration::Instance()->GetRenderAspectRatio();

		if (0 != sprites.size())
		{
			// Unlock vertex buffer.
			ASSERT(nullptr != VB.vertices);
			VB.vertices->Unmap();
			pVertices = nullptr;

			// Bind buffers.
			gD3D->BindVertexBuffer(0, VB.vertices, sizeof(SpriteVertex));
			gD3D->BindInputLayout(VB.inputLayout);

			sprites.sort();

			DWORD vbIdx = 0;
			for (Sprite sprite : sprites)
			{
				// @plek: This seems unnecessary?
				Vector2 aspect_corrected_scale = sprite.size;
				aspect_corrected_scale.y *= renderAspect;
			
				effect.SetVariableValue(varIndexTextureMap, sprite.pTexture->GetShaderResourceView());
				effectPass.Apply();

				gD3D->SetBlendMode(sprite.blendMode);

				gD3D->DrawTriQuad(vbIdx);
				vbIdx += 6;
			}
		}

		sprites.clear();
	}
}
