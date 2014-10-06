#include "Sprites.h"
#include "D3D.h"
#include "Shaders/Shader_Sprites.h"
#include "Configuration.h"

namespace Pimp
{
	Sprites::Sprites() :
		effect((unsigned char*)gCompiledShader_Sprites, sizeof(gCompiledShader_Sprites)),
		effectTechnique(&effect, "Sprites"),
		effectPass(&effectTechnique, "Default")
	{
		int varIndexRenderScale = effect.RegisterVariable("renderScale", true);

		const Vector2& visible_area = gD3D->GetRenderScale();
		effect.SetVariableValue(varIndexRenderScale, visible_area);

		varIndexPosition = effect.RegisterVariable("position", true);
		varIndexSize = effect.RegisterVariable("size", true);
		varIndexTextureMap = effect.RegisterVariable("textureMap", true);
	}

	Sprites::~Sprites()
	{
		for (Sprite *pSprite : sprites)
			delete pSprite;
	}

	void Sprites::AddSprite( Sprite* sprite )
	{
		sprites.push_back(sprite);
	}

	void Sprites::RemoveSprite( Sprite* sprite )
	{
		std::vector<Sprite*>::const_iterator found = std::find(sprites.begin(), sprites.end(), sprite);

		ASSERT(found != sprites.end());

		if (found != sprites.end())
		{
			sprites.erase(found);
			delete sprite;
		}
	}

	void Sprites::Render()
	{
		// TODO: Use some batching to draw the sprites? And use a dynamic vertex buffer?
		//       This currently just renders a full screen quad per sprite :)

		float renderAspect = Configuration::Instance()->GetRenderAspectRatio();

		for (Sprite *pSprite : sprites)
		{
			Vector2 aspect_corrected_scale = pSprite->GetSize();
			aspect_corrected_scale.y *= renderAspect;

			effect.SetVariableValue(varIndexPosition, pSprite->GetPosition());
			effect.SetVariableValue(varIndexSize, aspect_corrected_scale);
			effect.SetVariableValue(varIndexTextureMap, pSprite->GetTexture()->GetShaderResourceView());
			effectPass.Apply();	

			gD3D->DrawScreenQuad();
		}
	}
}