#pragma once

#include <vector>
#include <list>

#include <math/math.h>
#include "Effect.h"
#include "EffectPass.h"
#include "EffectTechnique.h"

namespace Pimp
{
	class Texture2D;

	class Sprites
	{
	public:
		Sprites();
		~Sprites();

		struct Sprite
		{
			/* const */ Texture2D *pTexture;
			D3D::BlendMode blendMode;
			Vector2 position, size;
			float sortZ;
			size_t vertexOffs;

			// std::sort() predicate
			bool operator <(const Sprite &RHS) const {
				return sortZ < RHS.sortZ;
			}
		};

		void AddSprite(
			/* const */ Texture2D *pTexture,
			D3D::BlendMode blendMode,
			const unsigned int vertexColor,
			const Vector2 &topLeft,
			const Vector2 &size,
			float sortZ,
			float rotateZ = 0.f);

		// simplified AddSprite()
		void AddSprite(
			/* const */ Texture2D *pTexture,
			D3D::BlendMode blendMode,
			const Vector2 &topLeft,
			float sortZ,
			float alpha = 1.f)
		{
			ASSERT(NULL != pTexture);
			const Vector2 size((float) pTexture->GetWidth(), (float) pTexture->GetHeight());
			const unsigned char iAlpha = int(alpha*255.f);
			AddSprite(pTexture, blendMode, iAlpha<<24 | 0xffffff, topLeft, size, sortZ);
		}

		// AddSprite() simplified & centered
		void AddSpriteCenter(
			/* const */ Texture2D *pTexture,
			Pimp::D3D::BlendMode blendMode,
			const Vector2 &center,
			float sortZ,
			float alpha = 1.f)
		{
			ASSERT(NULL != pTexture);
			const Vector2 size((float) pTexture->GetWidth(), (float) pTexture->GetHeight());
			const Vector2 topLeft = center - size*0.5f;
			const unsigned char iAlpha = int(alpha*255.f);
			AddSprite(pTexture, blendMode, iAlpha<<24 | 0xffffff, topLeft, size, sortZ);
		}

		void Flush();		

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

			void Bind();
		} VB;

		std::list<Sprite> sprites;

		struct SpriteVertex
		{
			Vector3 position;
			unsigned int ARGB;
			Vector2 UV;
		} *pVertices;

		Effect effect;
		EffectTechnique effectTechnique;
		EffectPass effectPass;

		int varIndexTextureMap;
	};
}