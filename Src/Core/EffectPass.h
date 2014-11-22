
#pragma once

#include "EffectTechnique.h"

namespace Pimp
{
	class EffectPass
	{
	public:
		EffectPass(EffectTechnique* effectTechnique, const char* passName);

		void Apply();

		void GetVSInputSignature(unsigned char** outSignature, int* outSignatureLength);

		Effect* GetEffect() const { return technique->GetEffect(); }

	private:
		EffectTechnique* technique;
		ID3D10EffectPass* pass;
	};
}
