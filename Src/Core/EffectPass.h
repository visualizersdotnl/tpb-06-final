
#pragma once

#include "EffectTechnique.h"

namespace Pimp
{
	class EffectPass
	{
	public:
		EffectPass(const EffectTechnique& technique, const char* passName);

		void Apply();
		void GetVSInputSignature(unsigned char** outSignature, int* outSignatureLength) const;

	private:
		ID3DX11EffectPass* pass;
	};
}
