
#pragma once

#include "Effect.h"

namespace Pimp
{
	class EffectTechnique
	{
	public:
		EffectTechnique(const Effect& effect, const char* techniqueName);

		ID3DX11EffectPass* GetPass(const char* passName);

	private:
		ID3DX11EffectTechnique* technique;
	};
}
