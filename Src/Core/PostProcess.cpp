#include "PostProcess.h"
#include "D3D.h"
#include "Shaders/Shader_PostProcess.h"
#include "Scene.h"
#include "Material.h"


// Should match the number of samples in the Shader_PostProcess.fx file
#define POSTPROCESS_BLOOMBLUR_NUMSAMPLES 15

// Scaledown factor for the filter buffer relative to the fullscreen buffer's size
#define POSTPROCESS_FILTER_SCALEDOWN 4

namespace Pimp
{

PostProcess::PostProcess() :
	effect((unsigned char*)gCompiledShader_PostProcess, sizeof(gCompiledShader_PostProcess)),
	techniquePostFX(&effect, "PostFX"),
	passBloomGather(&techniquePostFX, "Gather"),
	passBloomBlur(&techniquePostFX, "Blur"),
	passBloomCombine(&techniquePostFX, "Combine"),
	passMotionBlurBlend(&techniquePostFX, "MotionBlur"),
	userPostEffect(NULL)
{
	renderTargetSceneMS = gD3D->CreateRenderTarget(1, DXGI_FORMAT_R16G16B16A16_FLOAT, true);
	renderTargetSceneSingle = gD3D->CreateRenderTarget(1, DXGI_FORMAT_R16G16B16A16_FLOAT, false);
	renderTargetSceneMotionBlurred = gD3D->CreateRenderTarget(1, DXGI_FORMAT_R16G16B16A16_FLOAT, false);
	renderTargetSceneUserPostEffect = gD3D->CreateRenderTarget(1, DXGI_FORMAT_R16G16B16A16_FLOAT, false);
	
	for (int i=0; i<2; ++i)
		renderTargetFilter[i] = gD3D->CreateRenderTarget(POSTPROCESS_FILTER_SCALEDOWN, DXGI_FORMAT_R16G16B16A16_FLOAT, false);

	// Register variables
	varIndexScreenSizeInv = effect.RegisterVariable("screenSizeInv", true);
	varIndexFilterSizeInv = effect.RegisterVariable("filterSizeInv", true);
	varIndexBufferSceneColor = effect.RegisterVariable("bufferSceneColor", true);
	varIndexBufferFilter = effect.RegisterVariable("bufferFilter", true);
	varIndexBloomGatherSamples = effect.RegisterVariable("bloomGatherSamples", true);
	varIndexBloomBlurSamples = effect.RegisterVariable("bloomBlurSamples", true);
	varIndexBloomBlurWeights = effect.RegisterVariable("bloomBlurWeights", true);
	varIndexBloomBlurPixelDir = effect.RegisterVariable("bloomBlurPixelDir", true);
	varIndexLoadProgress = effect.RegisterVariable("loadProgress", true);
	varIndexMotionBlurWeight = effect.RegisterVariable("motionBlurFrameWeight", true);

	SetMotionBlurFrameWeight(1.0f);
	SetParameters();
}


PostProcess::~PostProcess()
{
	delete renderTargetSceneMS;
	delete renderTargetSceneSingle;
	delete renderTargetSceneMotionBlurred;
	delete renderTargetSceneUserPostEffect;
	delete renderTargetFilter[0];
	delete renderTargetFilter[1];
}


void PostProcess::OnSceneRenderLODChanged()
{
	SetParameters();
}


void PostProcess::SetParameters()
{
	float sceneRenderLod = Scene::GetSceneRenderLOD();

	DWORD width, height;
	gD3D->GetRenderSize(width, height);

	// Inverse render target size (XY) plus homogenous LOD UV adjustment (ZW).
	const Vector4 screenSizeInv(
		sceneRenderLod / width, 
		sceneRenderLod / height,
		0.5f*(1.0f - sceneRenderLod),
		0.5f*(1.0f - sceneRenderLod));
	effect.SetVariableValue(varIndexScreenSizeInv, screenSizeInv);

	// Inverse filter buffer size.
	const Vector2 invFilterSize(
		POSTPROCESS_FILTER_SCALEDOWN / (float) width,
		POSTPROCESS_FILTER_SCALEDOWN / (float) height);
	effect.SetVariableValue(varIndexFilterSizeInv, invFilterSize);

	effect.SetVariableValue(varIndexLoadProgress, 0.0f);

	InitBloomGatherSamples();
	InitBloomBlurSamples(invFilterSize);
}

void PostProcess::BindForRenderScene()
{
	gD3D->BindRenderTarget(renderTargetSceneMS, gD3D->GetDefaultDepthStencil());
}

void PostProcess::RenderPostProcess()
{
	// Resolve MS target to single sample
	renderTargetSceneMS->ResolveTo(renderTargetSceneSingle);

	// Apply motion blurring (just blend renderTargetSceneSingle on renderTargetSceneMotionBlurred)
	gD3D->SetBlendMode(D3D::BM_AlphaBlend);
	gD3D->BindRenderTarget(renderTargetSceneMotionBlurred, NULL);
	effect.SetVariableValue(varIndexBufferSceneColor, (ID3D10ShaderResourceView*)renderTargetSceneSingle->GetShaderResourceView());
	passMotionBlurBlend.Apply();
	gD3D->DrawScreenQuad();
	gD3D->SetBlendMode(D3D::BM_None);

	RenderTarget* gatherSource;

	if (userPostEffect != NULL)
	{
		// Apply user post effect
		gD3D->BindRenderTarget(renderTargetSceneUserPostEffect, NULL);
		userPostEffect->SetSceneBuffer(renderTargetSceneMotionBlurred->GetShaderResourceView());
		userPostEffect->Bind(NULL);
		gD3D->DrawScreenQuad();

		gatherSource = renderTargetSceneUserPostEffect;
	}
	else
		gatherSource = renderTargetSceneMotionBlurred;

	// Gather bloom
	gD3D->BindRenderTarget(renderTargetFilter[0], NULL);
	effect.SetVariableValue(varIndexBufferSceneColor, gatherSource->GetShaderResourceView());
	passBloomGather.Apply();	
	gD3D->DrawScreenQuad();

	// Blur pass 1 (H)
	gD3D->BindRenderTarget(renderTargetFilter[1], NULL);
	effect.SetVariableValue(varIndexBufferFilter, (ID3D10ShaderResourceView*)renderTargetFilter[0]->GetShaderResourceView());
	effect.SetVariableValue(varIndexBloomBlurPixelDir, bloomBlurDirH);
	passBloomBlur.Apply();
	gD3D->DrawScreenQuad();

	// Blur pass 2 (V)
	gD3D->BindRenderTarget(renderTargetFilter[0], NULL);
	effect.SetVariableValue(varIndexBufferFilter, (ID3D10ShaderResourceView*)renderTargetFilter[1]->GetShaderResourceView());
	effect.SetVariableValue(varIndexBloomBlurPixelDir, bloomBlurDirV);
	passBloomBlur.Apply();	
	gD3D->DrawScreenQuad();

	// Combine bloom results to back buffer:

	// 1. Bind and clear entire back buffer
	gD3D->BindBackbuffer(NULL);
	gD3D->SetBackViewport();
	gD3D->ClearBackBuffer();

	// 2: Combine into adjusted viewport (letterboxed) back buffer
	gD3D->SetBackAdjViewport();
	effect.SetVariableValue(varIndexBufferFilter, renderTargetFilter[0]->GetShaderResourceView());
	passBloomCombine.Apply();	
	gD3D->DrawScreenQuad();

	// Reset bound variables again
	effect.SetVariableValue(varIndexBufferSceneColor, (ID3D10ShaderResourceView*)NULL);
	effect.SetVariableValue(varIndexBufferFilter, (ID3D10ShaderResourceView*)NULL);
	passBloomCombine.Apply();
}

void PostProcess::InitBloomGatherSamples()
{
	DWORD width, height;
	gD3D->GetRenderSize(width, height);

	const float xStep = 1.f / width;
	const float yStep = 1.f / height;

	Vector4 packedOffsets[2];
	packedOffsets[0].x = -xStep;
	packedOffsets[0].y = -yStep;
	packedOffsets[0].w = -xStep;
	packedOffsets[0].z =  yStep;
	packedOffsets[1].x =  xStep;
	packedOffsets[1].y = -yStep;
	packedOffsets[1].w =  xStep;
	packedOffsets[1].z =  yStep;

	effect.SetVariableValue(varIndexBloomGatherSamples, packedOffsets, 2);
}


void PostProcess::InitBloomBlurSamples(Vector2 filterSizeInv)
{
	ASSERT(POSTPROCESS_BLOOMBLUR_NUMSAMPLES%2 == 1);
	ASSERT(POSTPROCESS_BLOOMBLUR_NUMSAMPLES >= 3);

	Vector4 offsets[POSTPROCESS_BLOOMBLUR_NUMSAMPLES];
	float weights[POSTPROCESS_BLOOMBLUR_NUMSAMPLES];

	int roundedKernelSize = (POSTPROCESS_BLOOMBLUR_NUMSAMPLES-1)/2;
	float blurKernelSize = (float)roundedKernelSize;

	int numSamples = 0;

	const float stdDev = sqrtf(blurKernelSize);
	const float scale = 1.0f / (stdDev * sqrtf(2.0f * M_PI));
	float weightSum = 0;

	for (int sampleIndex=-roundedKernelSize; sampleIndex<=roundedKernelSize; ++sampleIndex)
	{
		float weight0 = expf( -(float)(sampleIndex*sampleIndex) / (2.0f*blurKernelSize) ) * scale;
		float weight1 = expf( -(float)((sampleIndex+1)*(sampleIndex+1)) / (2.0f*blurKernelSize) ) * scale;
		float totalWeight = weight0 + weight1;

		ASSERT(numSamples < POSTPROCESS_BLOOMBLUR_NUMSAMPLES);
		offsets[numSamples].x = sampleIndex + weight1 / totalWeight;
		offsets[numSamples].y = offsets[numSamples].x;
		weights[numSamples] = totalWeight;

		++numSamples;
		weightSum += totalWeight;
	}

	ASSERT(numSamples == POSTPROCESS_BLOOMBLUR_NUMSAMPLES);

	for (int i=0; i<POSTPROCESS_BLOOMBLUR_NUMSAMPLES; ++i)
		weights[i] /= weightSum;

	effect.SetVariableValue(varIndexBloomBlurWeights, weights, POSTPROCESS_BLOOMBLUR_NUMSAMPLES);
	effect.SetVariableValue(varIndexBloomBlurSamples, offsets, POSTPROCESS_BLOOMBLUR_NUMSAMPLES);


	// Set filter directions
	bloomBlurDirH.x = filterSizeInv.x;
	bloomBlurDirH.y = 0;
	bloomBlurDirV.x = 0;
	bloomBlurDirV.y = filterSizeInv.y;
}


void PostProcess::Clear()
{
	gD3D->Clear(renderTargetSceneMS->GetRenderTargetView());
}


void PostProcess::SetUserPostEffect(Material* newPostEffect)
{
	userPostEffect = newPostEffect;
}

void PostProcess::SetLoadProgress( float v )
{
	effect.SetVariableValue(varIndexLoadProgress, v);
}

void PostProcess::SetMotionBlurFrameWeight(float w)
{
	this->w = w;
	effect.SetVariableValue(varIndexMotionBlurWeight, w);	
}


} // namespace
