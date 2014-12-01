
#pragma once

#include "EffectPass.h"
#include "RenderTarget.h"
#include "ScreenQuad.h"

namespace Pimp
{
	class Material;

	class PostProcess
	{
	public:
		PostProcess();
		~PostProcess();

		void Clear();
		void BindForRenderScene();
		void RenderPostProcess();

		void OnSceneRenderLODChanged();

		// User post effect shader is applied after motion blur and before bloom
		void SetUserPostEffect(Material* postEffect) { userPostEffect = postEffect; }
		Material* GetUserPostEffect() const          { return userPostEffect;       }

		// Loading bar progress (be sure it's zero once demo has started, FIXME)
		void SetLoadProgress(float progress);

		// Sets the weight of the current frame
		void SetMotionBlurFrameWeight(float weight);

	private:

		void SetParameters();
		void InitBloomGatherSamples();
		void InitBloomBlurSamples(Vector2 filterSizeInv);

		RenderTarget* renderTargetSceneMS;
		RenderTarget* renderTargetSceneSingle;
		RenderTarget* renderTargetSceneMotionBlurred;
		RenderTarget* renderTargetSceneUserPostEffect;
		RenderTarget* renderTargetFilter[2];

		Effect effect;
		EffectTechnique techniquePostFX;
		EffectPass passBloomGather;
		EffectPass passBloomBlur;
		EffectPass passBloomCombine;
		EffectPass passMotionBlurBlend;

		ScreenQuad screenQuad;

		int varIndexFilterSizeInv;
		int varIndexBufferSceneColor;
		int varIndexBufferFilter;
		int varIndexBloomGatherSamples;
		int varIndexBloomBlurSamples;
		int varIndexBloomBlurWeights;
		int varIndexBloomBlurPixelDir;
		int varIndexMotionBlurWeight;
		int varIndexLoadProgress; 

		Vector2 bloomBlurDirH;
		Vector2 bloomBlurDirV;

		Material* userPostEffect;
		float motionBlurWeight;
	};
}
