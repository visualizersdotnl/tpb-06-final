#pragma once

#include "EffectPass.h"
#include "RenderTarget.h"
#include "Settings.h"
#include "Camera.h"

namespace Pimp
{
	class Material;

	class PostProcess
	{
	public:
		PostProcess();
		~PostProcess();

		void Clear(float clearR, float clearG, float clearB);
		void BindForRenderScene();
		void RenderPostProcess(const Camera::DOFSettings& dofSettings);

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

		RenderTarget* renderTargetSceneMS;
		RenderTarget* renderTargetSceneSingle;
		RenderTarget* renderTargetSceneMotionBlurred;
		RenderTarget* renderTargetSceneUserPostEffect;
		RenderTarget* renderTargetFilter[2];

		Effect effect;
		EffectTechnique techniquePostFX;
		EffectPass passDOFGather;
		EffectPass passBloomGather;
		EffectPass passBlur;
		EffectPass passCombine;
		EffectPass passMotionBlurBlend;

		int varIndexScreenSizeInv;
		int varIndexFilterSizeInv;
		int varIndexRenderScale;
		int varIndexBufferSceneColor;
		int varIndexBufferFilter;
		int varIndexGatherSamples;
		int varIndexBlurSamples;
		int varIndexBlurWeights;
		int varIndexBlurPixelDir;
		int varIndexLoadProgress; // loadProgress
		int varIndexMotionBlurWeight;

		int varIndexDOFDepthFocus;
		int varIndexDOFDepthFar;
		int varIndexDOFDepthNear;

		Vector2 bloomBlurDirH;
		Vector2 bloomBlurDirV;

		Material* userPostEffect;
		float w;
	};
}
