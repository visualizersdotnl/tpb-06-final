
#include "Platform.h"
#include "D3D.h"
#include "EffectPass.h"

namespace Pimp
{
	EffectPass::EffectPass(const EffectTechnique& technique, const char* passName) :
		pass(technique.GetPass(passName))
	{
	}

	void EffectPass::Apply()
	{
		const HRESULT hResult = pass->Apply(0, gD3D->GetContext());
		D3D_ASSERT(hResult);
	}

	void EffectPass::GetVSInputSignature(unsigned char** outSignature, int* outSignatureLength) const
	{
		D3DX11_PASS_DESC passDesc;
		const HRESULT hRes = pass->GetDesc(&passDesc);
		D3D_ASSERT(hRes);

		ASSERT(nullptr != passDesc.pIAInputSignature);
		ASSERT(passDesc.IAInputSignatureSize > 0);

		*outSignature = new unsigned char[passDesc.IAInputSignatureSize];
		memcpy(*outSignature, passDesc.pIAInputSignature, (size_t) passDesc.IAInputSignatureSize);
		*outSignatureLength = (int) passDesc.IAInputSignatureSize;
	}
}
