
#pragma once

#include "Effect.h"

namespace Pimp
{
	class EffectTechnique
	{
	public:
		EffectTechnique(Effect* effect, const char* techniqueName);

		ID3D10EffectPass* GetPass(const char* passName);

		Effect* GetEffect() const { return effect; }

	private:
		Effect* effect;
		ID3D10EffectTechnique* technique;
	};
}
