#pragma once

#include "EffectPass.h"
#include "RenderTarget.h"
#include "Settings.h"
#include "ScreenQuadVertexBuffer.h"

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

		// Set background loading texture
		void SetLoadingTexture(Texture2D *texture);

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

		int varIndexScreenSizeInv;
		int varIndexFilterSizeInv;
		int varIndexRenderScale;
		int varIndexBufferSceneColor;
		int varIndexBufferFilter;
		int varIndexBloomGatherSamples;
		int varIndexBloomBlurSamples;
		int varIndexBloomBlurWeights;
		int varIndexBloomBlurPixelDir;
		int varIndexMotionBlurWeight;

		int varIndexLoadProgress; // loadProgress
		int varIndexLoadTexture; // 1920*1080 background texture
		Texture2D *loadingTexture;

		Vector2 bloomBlurDirH;
		Vector2 bloomBlurDirV;

		ScreenQuadVertexBuffer *quadVB;

		Material* userPostEffect;
		float w;
	};
}
