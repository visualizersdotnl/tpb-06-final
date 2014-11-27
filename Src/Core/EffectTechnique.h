
#pragma once

#include "Effect.h"

namespace Pimp
{
	class EffectTechnique
	{
	public:
		EffectTechnique(Effect* effect, const char* techniqueName);

		ID3DX11EffectPass* GetPass(const char* passName);
		Effect* GetEffect() const { return effect; }

	private:
		Effect* effect;
		ID3DX11EffectTechnique* technique;
	};
}
