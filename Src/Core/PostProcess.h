#pragma once

#include "EffectPass.h"
#include "RenderTarget.h"
#include "Settings.h"

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

		EffectPass* GetEffectPassBloomGather() 
		{ 
			return &passBloomGather; 
		}

		void OnSceneRenderLODChanged();

		void SetUserPostEffect(Material* newPostEffect);

		void SetLoadProgress(float v);

		Material* GetUserPostEffect() const
		{
			return userPostEffect;
		}

		// Sets the weight of the current frame.
		void SetMotionBlurFrameWeight(float w);

	private:

		void SetParameters();
		void InitBloomGatherSamples();
		void InitBloomBlurSamples(Vector2 filterSizeInv);
		void InitFlaresBlurSamples(Vector2 filterSizeInv);

		RenderTarget* renderTargetSceneMS;
		RenderTarget* renderTargetSceneSingle;
		RenderTarget* renderTargetSceneMotionBlurred;
		RenderTarget* renderTargetSceneUserPostEffect;
		RenderTarget* renderTargetFilter[3];

		Effect effect;
		EffectTechnique techniquePostFX;
		EffectPass passBloomGather;
		EffectPass passBloomBlur;
		EffectPass passFlaresBlur;
		EffectPass passBloomCombine;
		EffectPass passMotionBlurBlend;

		int varIndexScreenSizeInv;
		int varIndexFilterSizeInv;
		int varIndexRenderScale;
		int varIndexBufferSceneColor;
		int varIndexBufferFilterA;
		int varIndexBufferFilterB;
		int varIndexBloomGatherSamples;
		int varIndexBloomBlurSamples;
		int varIndexBloomBlurWeights;
		int varIndexBloomBlurPixelDir;
		int varIndexFlaresBlurSamples;
		int varIndexFlaresBlurWeights;
		int varIndexFlaresBlurPixelDir;
		int varIndexLoadProgress; // loadProgress
		int varIndexMotionBlurWeight;

		Vector2 bloomBlurDirH;
		Vector2 bloomBlurDirV;
		Vector2 flaresBlurDir;

		Material* userPostEffect;
		float w;
	};
}
