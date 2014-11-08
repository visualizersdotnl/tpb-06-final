
#include <algorithm>
#include <vector>

#include "Sprites.h"
#include "D3D.h"
#include "Shaders/Shader_Sprites.h"
#include "Configuration.h"

#include "../Player/Settings.h"

namespace Pimp
{
	const unsigned int kMaxSprites = 4096;

	Sprites::Sprites() :
		effect((unsigned char*)gCompiledShader_Sprites, sizeof(gCompiledShader_Sprites)),
		effectTechnique(&effect, "Sprites"),
		effectPass(&effectTechnique, "Default"), 
		effectPass_ForceClamp(&effectTechnique, "ForceClamp"),
		skipBGSprite2(true)
	{
		// Get shader var. indices.
		varIndexRenderScale = effect.RegisterVariable("renderScale", true);
		varIndexTextureMap = effect.RegisterVariable("textureMap", true);

		// Initialize buffers.
		VB.vertices = gD3D->CreateVertexBuffer(6*kMaxSprites*sizeof(SpriteVertex), nullptr, true);
		bgVB.vertices = gD3D->CreateVertexBuffer(2*6*sizeof(SpriteVertex), nullptr, true);

		sprites.clear();

		unsigned char* signature;
		int signatureLength;
		effectPass.GetVSInputSignature(&signature, &signatureLength);
		// ^ Identical for other effect pass.

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

		// Invalidate backgrounds.
		AddBackgroundSprite(0, gD3D->GetWhiteTex(), D3D::BlendMode::BM_None, 0, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));
		AddBackgroundSprite(1, gD3D->GetWhiteTex(), D3D::BlendMode::BM_None, 0, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));
	}

	Sprites::~Sprites()
	{
		// Member VB/bgVB takes care of it's resources.
	}

	inline const Vector3 Rotate(const Vector2 &position, const Vector2 &pivot, float angle)
	{
		// to convert back, as we get coordinates on X in [-aspectRatio, aspectRatio]
		const float aspectRatio = PIMPPLAYER_RENDER_ASPECT_RATIO;

		if (0.f == angle)
		{
			return Vector3(position.x/aspectRatio, position.y, 1.f);
		}
		else
		{
			const Vector2 renderScale = gD3D->GetRenderScale();
			const Matrix4 rotZ = CreateMatrixRotationZ(angle);
			const Vector2 adjPos = position - pivot;
			Vector3 rotPos = rotZ.TransformCoord(Vector3(adjPos.x, adjPos.y, 1.f));
			rotPos = rotPos + Vector3(pivot.x, pivot.y, 1.f);
			return Vector3(rotPos.x/aspectRatio, rotPos.y, 1.f);
		}
	}

	void Sprites::AddSprite(
			Texture2D *pTexture,
			D3D::BlendMode blendMode,
			const unsigned int vertexColor,
			const Vector2 &topLeft,
			const Vector2 &size,
			float sortZ,
			float rotateZ,
			bool forceClamp /* = false */,
			const Vector2 &uvTile /* = Vector2(1.f, 1.f) */,
			const Vector2 &uvScroll /* = Vector2(1.f, 1.f) */)
	{
		ASSERT(blendMode < D3D::BlendMode::MAX_BlendMode);
		ASSERT(sprites.size() < kMaxSprites);

//		ASSERT(sortZ == kBGSpriteZ[0] || sortZ == kBGSpriteZ[1] || sortZ >= 0.f);
		int iBG = -1;
		if (sortZ == kBGSpriteZ[0]) iBG = 0;
		if (sortZ == kBGSpriteZ[1]) iBG = 1;

		SpriteVertex *pDest = nullptr;

		if (iBG == -1)
		{
			// first sprite?
			if (sprites.empty())
			{
				// yes: lock vertex buffer
				VERIFY(SUCCEEDED(VB.vertices->Map(D3D10_MAP_WRITE_DISCARD, 0, reinterpret_cast<void **>(&pVertices))));
			}

			pDest = pVertices;
			pVertices += 6;
		}
		else
		{
			// lock tiny vertex buffer
			VERIFY(SUCCEEDED(bgVB.vertices->Map(D3D10_MAP_WRITE_DISCARD, 0, reinterpret_cast<void **>(&pDest))));
			pDest += iBG*6;
		}

		// hack: transform from top-left aligned 1920*1080 to semi-homogenous (X is aspect ratio adjusted) space
		// transformed to true homogenous by Rotate() above
		const float aspectRatio = PIMPPLAYER_RENDER_ASPECT_RATIO;
		const float adjTopLeftX = -aspectRatio + (topLeft.x/1920.f)*aspectRatio*2.f;
		const float adjTopLeftY = 1.f - (topLeft.y/1080.f)*2.f;
		const Vector2 adjTopLeft(adjTopLeftX, adjTopLeftY);
		const Vector2 adjSize((size.x/1920.f)*aspectRatio*2.f, -size.y/1080.f*2.f);
		const Vector2 bottomRight = adjTopLeft + adjSize;
		const Vector2 quadPivot(adjTopLeft.x + adjSize.x*0.5f, adjTopLeft.y + adjSize.y*0.5f);
		const unsigned int ARGB = vertexColor;

		// triangle 1: bottom right
		pDest->position = Rotate(Vector2(bottomRight.x, bottomRight.y), quadPivot, rotateZ);
		pDest->ARGB = ARGB;
		pDest->UV = Vector2(1.f*uvTile.x, 1.f*uvTile.y)+uvScroll;
		++pDest;
		// triangle 1: bottom left
		pDest->position = Rotate(Vector2(adjTopLeft.x, bottomRight.y), quadPivot, rotateZ);
		pDest->ARGB = ARGB;
		pDest->UV = Vector2(0.f, 1.f*uvTile.y)+uvScroll;
		++pDest;
		// triangle 1: top left
		pDest->position = Rotate(Vector2(adjTopLeft.x, adjTopLeft.y), quadPivot, rotateZ);
		pDest->ARGB = ARGB;
		pDest->UV = Vector2(0.f, 0.f)+uvScroll;
		++pDest;
		// triangle 2: bottom right
		pDest->position = Rotate(Vector2(bottomRight.x, bottomRight.y), quadPivot, rotateZ);
		pDest->ARGB = ARGB;
		pDest->UV = Vector2(1.f*uvTile.x, 1.f*uvTile.y)+uvScroll;
		++pDest;
		// triangle 2: top left
		pDest->position = Rotate(Vector2(adjTopLeft.x, adjTopLeft.y), quadPivot, rotateZ);
		pDest->ARGB = ARGB;
		pDest->UV = Vector2(0.f, 0.f)+uvScroll;
		++pDest;
		// triangle 2: top right
		pDest->position = Rotate(Vector2(bottomRight.x, adjTopLeft.y), quadPivot, rotateZ);
		pDest->ARGB = ARGB;
		pDest->UV = Vector2(1.f*uvTile.x, 0.f)+uvScroll;
		++pDest;

		ASSERT(nullptr != pTexture);
		if (iBG == -1)
		{
			// add to list
			Sprite sprite;
			sprite.pTexture = pTexture;
			sprite.blendMode = blendMode;
			sprite.sortZ = sortZ;
			sprite.vertexOffs = sprites.size()*6;
			sprite.forceClamp = forceClamp;
			sprites.push_back(std::move(sprite));	
		}
		else
		{
			// set up background sprite
			bgSprites[iBG].pTexture = pTexture;
			bgSprites[iBG].blendMode = blendMode;

			// and unmap it's buffer
			bgVB.vertices->Unmap();
		}
	}

	void Sprites::DrawBackgroundSprite()
	{
		for (int iBG = 0; iBG < ((true == skipBGSprite2) ? 1 : 2); ++iBG)
		{
			// Bind buffers.
			gD3D->BindVertexBuffer(0, bgVB.vertices, sizeof(SpriteVertex));
			gD3D->BindInputLayout(VB.inputLayout); // FIXME: bgVB doesn't have one, as they're identical :)
		
			// Set state.
			effect.SetVariableValue(varIndexRenderScale, Vector2(1.f, 1.f)); // FIXME: *
			effect.SetVariableValue(varIndexTextureMap, bgSprites[iBG].pTexture->GetShaderResourceView());
			effectPass.Apply();
			gD3D->SetBlendMode(bgSprites[iBG].blendMode);

			// * - For some reason on the render targets the scale for the background sprites is too much as it is 
			//     already compensated elsewhere. Can't be bothered to find out where now. 
			//     @plek, 2014

			// Draw.
			gD3D->DrawTriQuad(iBG*6);
		}
	}

	void Sprites::FlushSprites()
	{
		if (0 != sprites.size())
		{
			// Unmap vertex buffer.
			ASSERT(nullptr != VB.vertices);
			VB.vertices->Unmap();
			pVertices = nullptr;

			// Bind buffers.
			gD3D->BindVertexBuffer(0, VB.vertices, sizeof(SpriteVertex));
			gD3D->BindInputLayout(VB.inputLayout);

			// Sort 'em.
			sprites.sort();

			// Set correct visible area.
			const Vector2& visible_area = gD3D->GetRenderScale();
//			effect.SetVariableValue(varIndexRenderScale, visible_area);
			effect.SetVariableValue(varIndexRenderScale, Vector2(1.f, 1.f)); // FIXME: *

			for (Sprite sprite : sprites)
			{
				// Set state.
				effect.SetVariableValue(varIndexTextureMap, sprite.pTexture->GetShaderResourceView());
				(false == sprite.forceClamp) ? effectPass.Apply() : effectPass_ForceClamp.Apply();
				gD3D->SetBlendMode(sprite.blendMode);

				// Draw.
				gD3D->DrawTriQuad((DWORD) sprite.vertexOffs);
			}

			sprites.clear();
		}
	}
}
