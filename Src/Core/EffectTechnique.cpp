
#include "Platform.h"
#include "D3D.h"
#include "EffectTechnique.h"

namespace Pimp
{
	EffectTechnique::EffectTechnique(Effect* effect, const char* techniqueName)
		: effect(effect)
	{
		technique = effect->GetTechnique(techniqueName);
		ASSERT(nullptr != technique);
	}

	ID3D10EffectPass* EffectTechnique::GetPass(const char* passName)
	{
		ID3D10EffectPass* pass = technique->GetPassByName(passName);
		ASSERT(pass->IsValid());
		return pass;
	}
}
