
#pragma once

#include "EffectTechnique.h"

namespace Pimp
{
	class EffectPass
	{
	public:
		EffectPass(EffectTechnique& technique, const char* passName);

		void GetVSInputSignature(unsigned char** outSignature, int* outSignatureLength);
		void Apply();

	private:
//		EffectTechnique& technique;
		ID3DX11EffectPass* pass;
	};
}
