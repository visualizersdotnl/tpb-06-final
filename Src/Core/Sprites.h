#pragma once

#include <list>
#include <math/math.h>
#include "Effect.h"
#include "EffectPass.h"
#include "EffectTechnique.h"

namespace Pimp
{
	class Texture2D;

	// This sortZ is reserved.
	// The idea is to keep your regular sortZ zero and upwards.
	const float kBGSpriteZ = -1.f;

	class Sprites
	{
	private:
		struct SpriteVertex
		{
			Vector3 position;
			unsigned int ARGB;
			Vector2 UV;
		} *pVertices;

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
			const Vector2 &uvTile = Vector2(1.f, 1.f));

		// to add (single) background sprite (drawn first, behind the scene)
		void AddBackgroundSprite(
			/* const */ Texture2D *pTexture,
			D3D::BlendMode blendMode,
			const unsigned int vertexColor,
			const Vector2 &topLeft,
			const Vector2 &size,
			const Vector2 &uvTile)
		{
			AddSprite(pTexture, blendMode, vertexColor, topLeft, size, kBGSpriteZ, 0.f, uvTile);
		}

		// simplified AddSprite()
		void AddSprite(
			/* const */ Texture2D *pTexture,
			D3D::BlendMode blendMode,
			const Vector2 &topLeft,
			float sortZ,
			float alpha,
			float rotateZ)
		{
			ASSERT(NULL != pTexture);
			const Vector2 size((float) pTexture->GetWidth(), (float) pTexture->GetHeight());
			const unsigned char iAlpha = int(alpha*255.f);
			AddSprite(pTexture, blendMode, iAlpha<<24 | 0xffffff, topLeft, size, sortZ, rotateZ);
		}

		// AddSprite() simplified & centered
		void AddSpriteCenter(
			/* const */ Texture2D *pTexture,
			Pimp::D3D::BlendMode blendMode,
			const Vector2 &center,
			float sortZ,
			float alpha,
			float rotateZ)
		{
			ASSERT(NULL != pTexture);
			const Vector2 size((float) pTexture->GetWidth(), (float) pTexture->GetHeight());
			const Vector2 topLeft = center - size*0.5f;
			const unsigned char iAlpha = int(alpha*255.f);
			AddSprite(pTexture, blendMode, iAlpha<<24 | 0xffffff, topLeft, size, sortZ, rotateZ);
		}

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

		BGSprite bgSprite;
		std::list<Sprite> sprites;

		Effect effect;
		EffectTechnique effectTechnique;
		EffectPass effectPass;

		int varIndexTextureMap;
	};
}