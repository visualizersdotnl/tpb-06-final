
#include "Platform.h"
#include "Sprites.h"
#include "Shaders/Shader_Sprites.h"

namespace Pimp
{
	const unsigned int kMaxSprites = 4096;

	Sprites::Sprites(float vResX, float vResY) :
		vResX(vResX), vResY(vResY)
,		pMapped(nullptr)
,		effect((unsigned char*)gCompiledShader_Sprites, sizeof(gCompiledShader_Sprites))
,		effectTechnique(&effect, "Sprites")
,		effectPassWrap(&effectTechnique, "Wrap")
,		effectPassClamp(&effectTechnique, "Clamp")
	{
		// Get shader var. indices.
		varIndexTextureMap = effect.RegisterVariable("textureMap", true);

		// Create VB & layout.
		VB.buffer = gD3D->CreateVertexBuffer(6*kMaxSprites*sizeof(Vertex), nullptr, true);

		unsigned char* signature;
		int signatureLength;
		effectPassWrap.GetVSInputSignature(&signature, &signatureLength);
		// ^ Signatures are identical for both.

		D3D11_INPUT_ELEMENT_DESC elemDesc[3];
		elemDesc[0].SemanticName = "POSITION";
		elemDesc[0].SemanticIndex = 0;
		elemDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		elemDesc[0].InputSlot = 0;
		elemDesc[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elemDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elemDesc[0].InstanceDataStepRate = 0;
		elemDesc[1].SemanticName = "COLOR";
		elemDesc[1].SemanticIndex = 0;
		elemDesc[1].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		elemDesc[1].InputSlot = 0;
		elemDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elemDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elemDesc[1].InstanceDataStepRate = 0;
		elemDesc[2].SemanticName = "TEXCOORD";
		elemDesc[2].SemanticIndex = 0;
		elemDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		elemDesc[2].InputSlot = 0;
		elemDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elemDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elemDesc[2].InstanceDataStepRate = 0;
		VB.layout = gD3D->CreateInputLayout(elemDesc, 3, signature, signatureLength);

		delete [] signature;
	}

	inline const Vector3 Rotate(const Vector2 &position, const Vector2 &pivot, float angle, float aspectRatio)
	{
		if (0.f == angle)
		{
			// use aspectRatio to convert back, as we get coordinates on X in [-aspectRatio, aspectRatio]
			return Vector3(position.x/aspectRatio, position.y, 1.f);
		}
		else
		{
			const Matrix4 rotZ = CreateMatrixRotationZ(angle);
			const Vector2 adjPos = position - pivot;
			Vector3 rotPos = rotZ.TransformCoord(Vector3(adjPos.x, adjPos.y, 1.f));
			rotPos = rotPos + Vector3(pivot.x, pivot.y, 1.f);
			return Vector3(rotPos.x/aspectRatio, rotPos.y, 1.f);
		}
	}

	void Sprites::AddSprite(
			Texture2D *pTexture,
			D3D::Blend blendMode,
			const unsigned int vertexColor,
			const Vector2 &topLeft,
			const Vector2 &size,
			float sortZ,
			float rotateZ,
			bool forceClamp,
			bool isBackground,
			const Vector2 &uvTile,  /* = Vector2(1.f, 1.f) */
			const Vector2 &uvScroll /* = Vector2(1.f, 1.f) */)
	{
		ASSERT(blendMode < D3D::Blend::MAX_BlendMode);
		ASSERT(sprites.size() < kMaxSprites);

		// allocate vertices
		Vertex *pDest = nullptr;
		if (nullptr == pMapped)
		{
			// lock vertex buffer
			D3D11_MAPPED_SUBRESOURCE mappedVB;
			VERIFY(SUCCEEDED(gD3D->GetContext()->Map(VB.buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB)));
			pMapped = reinterpret_cast<Vertex*>(mappedVB.pData);
			curMappedVtx = 0;
		}

		pDest = pMapped + curMappedVtx;

		// transform from top-left aligned vResX*vResY to semi-homogenous (X is aspect ratio adjusted) space,
		// transformed to true homogenous by Rotate() above
		const float aspectRatio = vResX/vResY;
		const float adjTopLeftX = -aspectRatio + (topLeft.x/vResX)*aspectRatio*2.f;
		const float adjTopLeftY = 1.f - (topLeft.y/vResY)*2.f;
		const Vector2 adjTopLeft(adjTopLeftX, adjTopLeftY);
		const Vector2 adjSize((size.x/vResX)*aspectRatio*2.f, -size.y/vResY*2.f);
		const Vector2 bottomRight = adjTopLeft + adjSize;
		const Vector2 quadPivot(adjTopLeft.x + adjSize.x*0.5f, adjTopLeft.y + adjSize.y*0.5f);
		const unsigned int ARGB = vertexColor;

		// triangle 1: bottom right
		pDest->position = Rotate(Vector2(bottomRight.x, bottomRight.y), quadPivot, rotateZ, aspectRatio);
		pDest->ARGB = ARGB;
		pDest->UV = Vector2(1.f*uvTile.x, 1.f*uvTile.y)+uvScroll;
		++pDest;
		// triangle 1: bottom left
		pDest->position = Rotate(Vector2(adjTopLeft.x, bottomRight.y), quadPivot, rotateZ, aspectRatio);
		pDest->ARGB = ARGB;
		pDest->UV = Vector2(0.f, 1.f*uvTile.y)+uvScroll;
		++pDest;
		// triangle 1: top left
		pDest->position = Rotate(Vector2(adjTopLeft.x, adjTopLeft.y), quadPivot, rotateZ, aspectRatio);
		pDest->ARGB = ARGB;
		pDest->UV = Vector2(0.f, 0.f)+uvScroll;
		++pDest;
		// triangle 2: bottom right
		pDest->position = Rotate(Vector2(bottomRight.x, bottomRight.y), quadPivot, rotateZ, aspectRatio);
		pDest->ARGB = ARGB;
		pDest->UV = Vector2(1.f*uvTile.x, 1.f*uvTile.y)+uvScroll;
		++pDest;
		// triangle 2: top left
		pDest->position = Rotate(Vector2(adjTopLeft.x, adjTopLeft.y), quadPivot, rotateZ, aspectRatio);
		pDest->ARGB = ARGB;
		pDest->UV = Vector2(0.f, 0.f)+uvScroll;
		++pDest;
		// triangle 2: top right
		pDest->position = Rotate(Vector2(bottomRight.x, adjTopLeft.y), quadPivot, rotateZ, aspectRatio);
		pDest->ARGB = ARGB;
		pDest->UV = Vector2(1.f*uvTile.x, 0.f)+uvScroll;
		++pDest;

		ASSERT(nullptr != pTexture);

		// add to list
		Sprite sprite;
		sprite.pTexture = pTexture;
		sprite.blendMode = blendMode;
		sprite.sortZ = sortZ;
		sprite.vertexOffs = curMappedVtx;
		sprite.forceClamp = forceClamp;

		if (false == isBackground)
			sprites.push_back(std::move(sprite));	
		else
			bgSprites.push_back(std::move(sprite));

		// next quad
		curMappedVtx += 6;
	}

	void Sprites::PrepareToDraw()
	{
		if (nullptr != pMapped)
		{
			gD3D->GetContext()->Unmap(VB.buffer, 0);
			pMapped = nullptr;
		}
	}

	void Sprites::Flush(std::list<Sprite> &list)
	{
		if (false == list.empty())
		{
			ASSERT(nullptr == pMapped);

			// Bind VB.
			gD3D->BindVertexBuffer(0, VB.buffer, sizeof(Vertex));
			gD3D->BindInputLayout(VB.layout);

			// Sort 'em.
			list.sort();

			// Draw.
			for (Sprite sprite : list)
			{
				// Set state.
				effect.SetVariableValue(varIndexTextureMap, sprite.pTexture->GetShaderResourceView());
				(true == sprite.forceClamp) ? effectPassClamp.Apply() : effectPassWrap.Apply();
				gD3D->SetBlendMode(sprite.blendMode);

				// Draw.
				gD3D->DrawTriQuad((DWORD) sprite.vertexOffs);
			}

			list.clear();
		}
	}
}
