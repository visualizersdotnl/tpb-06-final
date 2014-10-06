#pragma once

#include <math/math.h>
#include <vector>
#include "Effect.h"
#include "EffectPass.h"
#include "EffectTechnique.h"

namespace Pimp
{
	class Texture2D;

	class Sprite
	{
	private:
		Vector2 position;
		Vector2 size;
		Texture2D* texture;

	public:
		Sprite(const Vector2& position, const Vector2& size, Texture2D* texture)
			: position(position), size(size), texture(texture) 
		{}

		const Vector2& GetPosition() const 
		{ 
			return position; 
		}
		void SetPosition(const Vector2& val) 
		{ 
			position = val; 
		}		

		const Vector2& GetSize() const 
		{ 
			return size; 
		}
		void SetSize(const Vector2& val) 
		{ 
			size = val; 
		}

		Texture2D* GetTexture() const 
		{ 
			return texture; 
		}
		void SetTexture(Texture2D* val) 
		{ 
			texture = val; 
		}
	};

	class Sprites
	{
	private:
		std::vector<Sprite*> sprites;	// Pointers to sprites. Owned by this class.

		Effect effect;
		EffectTechnique effectTechnique;
		EffectPass effectPass;

		int varIndexPosition;
		int varIndexSize;
		int varIndexTextureMap;

	public:
		Sprites();
		~Sprites();

		void AddSprite(Sprite* sprite);
		void RemoveSprite(Sprite* sprite);

		void Render();		
	};
}