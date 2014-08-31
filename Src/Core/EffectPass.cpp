#include "EffectPass.h"

namespace Pimp
{
	EffectPass::EffectPass(EffectTechnique* effectTechnique, const char* passName)
		: technique(effectTechnique)
	{
		pass = effectTechnique->GetPass(passName);
	}


	void EffectPass::Apply()
	{
		HRESULT hr = pass->Apply(0);
		D3D_ASSERT(hr);
	}

	void EffectPass::GetVSInputSignature(unsigned char** outSignature, int* outSignatureLength)
	{
		D3D10_PASS_DESC desc;
		HRESULT hr = pass->GetDesc(&desc);
		D3D_ASSERT(hr);

		ASSERT(desc.pIAInputSignature != NULL);
		ASSERT(desc.IAInputSignatureSize > 0);

		*outSignature = new unsigned char[desc.IAInputSignatureSize];
		*outSignatureLength = (int)desc.IAInputSignatureSize;

		memcpy(*outSignature, desc.pIAInputSignature, (unsigned int)desc.IAInputSignatureSize);
	}
}

