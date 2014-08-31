#include "EffectTechnique.h"

namespace Pimp
{
	EffectTechnique::EffectTechnique( Effect* effect, const char* techniqueName )
		: effect(effect)
	{
		technique = effect->GetTechnique(techniqueName);
		ASSERT(technique != NULL);
	}


	ID3D10EffectPass* EffectTechnique::GetPass(const char* passName)
	{
		ID3D10EffectPass* pass = technique->GetPassByName(passName);
		ASSERT(pass->IsValid());
		return pass;
	}
}

