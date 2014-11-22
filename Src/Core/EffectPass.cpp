
#include "Platform.h"
#include "D3D.h"
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
		const HRESULT hResult = pass->Apply(0);
		D3D_ASSERT(hResult);
	}

	void EffectPass::GetVSInputSignature(unsigned char** outSignature, int* outSignatureLength)
	{
		D3D10_PASS_DESC desc;
		const HRESULT hRes = pass->GetDesc(&desc);
		D3D_ASSERT(hRes);

		ASSERT(nullptr != desc.pIAInputSignature);
		ASSERT(desc.IAInputSignatureSize > 0);
		*outSignature = new unsigned char[desc.IAInputSignatureSize];
		memcpy(*outSignature, desc.pIAInputSignature, (size_t) desc.IAInputSignatureSize);
		*outSignatureLength = (int)desc.IAInputSignatureSize;
	}
}
