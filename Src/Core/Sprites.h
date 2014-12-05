
//
// Sprite system 1.0.
//
// Sprites can be either put in the background or post-scene queue.
// They use a virtual coordinate system supplied by Player.
// In a logical world it's aspect ratio matches the render aspect ratio.
//

#pragma once

#include "D3D.h"
#include "Effect.h"
#include "EffectPass.h"
#include "EffectTechnique.h"

namespace Pimp
{
	class Texture2D;

	class Sprites : public boost::noncopyable
	{
	private:
		struct Vertex
		{
			Vector3 position;
			unsigned int ARGB;
			Vector2 UV;
		}; 
		
		Vertex* pMapped;
		size_t curMappedVtx;

		struct Sprite
		{
			/* const */ Texture2D *pTexture;
			D3D::Blend blendMode;
			float sortZ;
			size_t vertexOffs;
			bool forceClamp;

			// std::sort() predicate
			bool operator <(const Sprite &RHS) const {
				return sortZ < RHS.sortZ;
			}
		};

	public:
		Sprites(float vResX, float vResY);
		~Sprites() {}

		// add post-scene sprite
		void AddSprite(
			/* const */ Texture2D *pTexture,
			D3D::Blend blendMode,
			unsigned int vertexColor,
			const Vector2 &topLeft,
			const Vector2 &size,
			float sortZ,
			float rotateZ,
			bool forceClamp,
			bool isBackground,
			const Vector2 &uvTile = Vector2(1.f, 1.f),
			const Vector2 &uvScroll = Vector2(0.f, 0.f));

		// add background sprite (drawn first, behind the scene shader & geometry)
		void AddBackgroundSprite(
			/* const */ Texture2D *pTexture,
			D3D::Blend blendMode,
			unsigned int vertexColor,
			const Vector2 &topLeft,
			const Vector2 &size,
			float sortZ,
			float rotateZ,
			bool forceClamp,
			const Vector2 &uvTile = Vector2(1.f, 1.f),
			const Vector2 &uvScroll = Vector2(0.f, 0.f))
		{
			AddSprite(pTexture, blendMode, vertexColor, topLeft, size, sortZ, rotateZ, forceClamp, true, uvTile, uvScroll);
		}

		// simplified AddBackgroundSprite()
		void AddBackgroundSprite(
			/* const */ Texture2D *pTexture,
			D3D::Blend blendMode,
			unsigned int vertexColor,
			float sortZ,
			bool forceClamp)
		{
			AddSprite(pTexture, blendMode, vertexColor, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), sortZ, 0.f, forceClamp, true);
		}

		// simplified AddSprite()
		void AddSprite(
			/* const */ Texture2D *pTexture,
			D3D::Blend blendMode,
			const Vector2 &topLeft,
			float sortZ,
			float alpha,
			float rotateZ,
			bool forceClamp)
		{
			ASSERT(NULL != pTexture);
			const Vector2 size((float) pTexture->GetWidth(), (float) pTexture->GetHeight());
			const unsigned char iAlpha = int(alpha*255.f);
			AddSprite(pTexture, blendMode, iAlpha<<24 | 0xffffff, topLeft, size, sortZ, rotateZ, forceClamp, false);
		}

		// AddSprite() simplified & centered
		void AddSpriteCenter(
			/* const */ Texture2D *pTexture,
			Pimp::D3D::Blend blendMode,
			const Vector2 &center,
			float sortZ,
			float alpha,
			float rotateZ,
			bool forceClamp)
		{
			ASSERT(NULL != pTexture);
			const Vector2 size((float) pTexture->GetWidth(), (float) pTexture->GetHeight());
			const Vector2 topLeft = center - size*0.5f;
			const unsigned char iAlpha = int(alpha*255.f);
			AddSprite(pTexture, blendMode, iAlpha<<24 | 0xffffff, topLeft, size, sortZ, rotateZ, forceClamp, false);
		}

		// Call before any of the Draw*() calls.
		void PrepareToDraw();

		void DrawBackgroundSprites() { Flush(bgSprites); }
		void DrawSprites()           { Flush(sprites);   }

	private:
		const float vResX, vResY;

		// FIXME: Simple combined VB & layout container, expose as a global object?
		class VertexBuffer
		{
		public:
			ID3D11Buffer* buffer;
			ID3D11InputLayout* layout;

			VertexBuffer() :
				buffer(nullptr),
				layout(nullptr) {}

			~VertexBuffer() 
			{ 
				SAFE_RELEASE(buffer);
				SAFE_RELEASE(layout);
			}
		} VB;

		std::list<Sprite> bgSprites;
		std::list<Sprite> sprites;

		Effect effect;
		EffectTechnique effectTechnique;
		EffectPass effectPassWrap, effectPassClamp;
		int varIndexTextureMap;

		void Flush(std::list<Sprite> &list);
	};
}
