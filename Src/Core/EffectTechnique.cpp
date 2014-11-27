
#include "Platform.h"
#include "D3D.h"
#include "EffectTechnique.h"

namespace Pimp
{
	EffectTechnique::EffectTechnique(const Effect& effect, const char* techniqueName)
	{
		technique = effect.GetTechnique(techniqueName);
		ASSERT(nullptr != technique);
	}

	ID3DX11EffectPass* EffectTechnique::GetPass(const char* passName)
	{
		ID3DX11EffectPass* pass = technique->GetPassByName(passName);
		ASSERT(nullptr != pass && pass->IsValid());
		return pass;
	}
}
