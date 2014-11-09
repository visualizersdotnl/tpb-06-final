#pragma once

#include <list>
#include <math/math.h>
#include "Effect.h"
#include "EffectPass.h"
#include "EffectTechnique.h"

namespace Pimp
{
	class Texture2D;

	// These sortZs are reserved.
	// The idea is to keep your regular sortZ zero and upwards.
	const float kBGSpriteZ[2] = { -1.f, -2.f };

	class Sprites
	{
	private:
		struct SpriteVertex
		{
			Vector3 position;
			unsigned int ARGB;
			Vector2 UV;
		} *pVertices, *pBGVertices;

		struct BGSprite
		{
			/* const */ Texture2D *pTexture;
			D3D::BlendMode blendMode;
		};

		struct Sprite
		{
			/* const */ Texture2D *pTexture;
			D3D::BlendMode blendMode;
			float sortZ;
			size_t vertexOffs;
			bool forceClamp;

			// std::sort() predicate
			bool operator <(const Sprite &RHS) const {
				return sortZ < RHS.sortZ;
			}
		};

	public:
		Sprites();
		~Sprites();

		void AddSprite(
			/* const */ Texture2D *pTexture,
			D3D::BlendMode blendMode,
			const unsigned int vertexColor,
			const Vector2 &topLeft,
			const Vector2 &size,
			float sortZ,
			float rotateZ,
			bool forceClamp = false,
			const Vector2 &uvTile = Vector2(1.f, 1.f),
			const Vector2 &uvScroll = Vector2(0.f, 0.f));

		// to add (single) background sprite (drawn first, behind the scene)
		void AddBackgroundSprite(
			size_t iBG,
			/* const */ Texture2D *pTexture,
			D3D::BlendMode blendMode,
			const unsigned int vertexColor,
			const Vector2 &topLeft,
			const Vector2 &size,
			const Vector2 &uvTile, 
			const Vector2 &uvScroll = Vector2(0.f, 0.f))
		{
			AddSprite(pTexture, blendMode, vertexColor, topLeft, size, kBGSpriteZ[iBG], 0.f, false, uvTile, uvScroll);
		}

		// simplified AddSprite()
		void AddSprite(
			/* const */ Texture2D *pTexture,
			D3D::BlendMode blendMode,
			const Vector2 &topLeft,
			float sortZ,
			float alpha,
			float rotateZ,
			bool forceClamp = false)
		{
			ASSERT(NULL != pTexture);
			const Vector2 size((float) pTexture->GetWidth(), (float) pTexture->GetHeight());
			const unsigned char iAlpha = int(alpha*255.f);
			AddSprite(pTexture, blendMode, iAlpha<<24 | 0xffffff, topLeft, size, sortZ, rotateZ, forceClamp);
		}

		// AddSprite() simplified & centered
		void AddSpriteCenter(
			/* const */ Texture2D *pTexture,
			Pimp::D3D::BlendMode blendMode,
			const Vector2 &center,
			float sortZ,
			float alpha,
			float rotateZ,
			bool forceClamp = false)
		{
			ASSERT(NULL != pTexture);
			const Vector2 size((float) pTexture->GetWidth(), (float) pTexture->GetHeight());
			const Vector2 topLeft = center - size*0.5f;
			const unsigned char iAlpha = int(alpha*255.f);
			AddSprite(pTexture, blendMode, iAlpha<<24 | 0xffffff, topLeft, size, sortZ, rotateZ, forceClamp);
		}

		// FIXME (ugly last-minute hack, add a proper list)
		bool skipBGSprite2;
		void SkipBGSprite2(bool skip) { skipBGSprite2 = skip; }

		void DrawBackgroundSprite();
		void FlushSprites();	

	private:
		class SpriteVertexBuffer
		{
		public:
			ID3D10Buffer* vertices;
			ID3D10InputLayout* inputLayout;

			SpriteVertexBuffer() :
				vertices(nullptr),
				inputLayout(nullptr) {}

			~SpriteVertexBuffer() 
			{ 
				if (nullptr != vertices) 
					vertices->Release(); 

				if (nullptr != inputLayout) 
					inputLayout->Release(); 
			}
		} VB, bgVB;

		BGSprite bgSprites[2];
		std::list<Sprite> sprites;

		Effect effect;
		EffectTechnique effectTechnique;
		EffectPass effectPass, effectPass_ForceClamp;

		int varIndexRenderScale;
		int varIndexTextureMap;
	};
}