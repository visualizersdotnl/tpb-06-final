#include "PostProcess.h"
#include "D3D.h"
#include "Shaders/Shader_PostProcess.h"
#include "Scene.h"
#include "Material.h"


// Should match the number of samples from the postprocess.fx file
#define POSTPROCESS_BLOOMBLUR_NUMSAMPLES 15

// Scaledown factor for the filter buffer relative to the fullscreen buffer's size
#define POSTPROCESS_FILTER_SCALEDOWN 4

namespace Pimp
{

PostProcess::PostProcess() :
	effect((unsigned char*)gCompiledShader_PostProcess, sizeof(gCompiledShader_PostProcess)),
	techniquePostFX(&effect, "PostFX"),
	passBloomGather(&techniquePostFX, "GatherBloom"),
	passDOFGather(&techniquePostFX, "GatherDOF"),
	passBlur(&techniquePostFX, "Blur"),
	passCombine(&techniquePostFX, "Combine"),
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
	varIndexRenderScale = effect.RegisterVariable("renderScale", true);
	varIndexBufferSceneColor = effect.RegisterVariable("bufferSceneColor", true);
	varIndexBufferFilter = effect.RegisterVariable("bufferFilter", true);
	varIndexGatherSamples = effect.RegisterVariable("gatherSamples", true);
	varIndexBlurSamples = effect.RegisterVariable("blurSamples", true);
	varIndexBlurWeights = effect.RegisterVariable("blurWeights", true);
	varIndexBlurPixelDir = effect.RegisterVariable("blurPixelDir", true);
	varIndexLoadProgress = effect.RegisterVariable("loadProgress", true);
	varIndexMotionBlurWeight = effect.RegisterVariable("motionBlurFrameWeight", true);

	varIndexDOFDepthFocus = effect.RegisterVariable("depthFocus", true);
	varIndexDOFDepthFar = effect.RegisterVariable("depthFarBlur", true);
	varIndexDOFDepthNear = effect.RegisterVariable("depthNearBlur", true);

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

	int w, h;
	gD3D->GetViewportSize(&w, &h);

	// Also take our scene render lod into account.
	// This maps the renderable part of our screen buffer to the whole screen quad.
	Vector4 invScreenSize(
		sceneRenderLod / (float)w, 
		sceneRenderLod / (float)h,
		0.5f*(1.0f - sceneRenderLod),
		0.5f*(1.0f - sceneRenderLod));
	effect.SetVariableValue(varIndexScreenSizeInv, invScreenSize);

	Vector2 invFilterSize(
		(float)POSTPROCESS_FILTER_SCALEDOWN / (float) w,
		(float)POSTPROCESS_FILTER_SCALEDOWN / (float) h);
	effect.SetVariableValue(varIndexFilterSizeInv, invFilterSize);

	effect.SetVariableValue(varIndexLoadProgress, 0.0f);

	InitBloomGatherSamples();
	InitBloomBlurSamples(invFilterSize);
}

void PostProcess::BindForRenderScene()
{
	gD3D->BindRenderTarget(renderTargetSceneMS, gD3D->GetDefaultDepthStencil());
}

void PostProcess::RenderPostProcess(const Camera::DOFSettings& dofSettings)
{
	// Resolve MS target to single sample
	renderTargetSceneMS->ResolveTo(renderTargetSceneSingle);

	const static Vector2 whole_screen_range(1,1);
	effect.SetVariableValue(varIndexRenderScale, whole_screen_range);

	//// Apply motion blurring (just blend renderTargetSceneSingle on renderTargetSceneMotionBlurred)
	//gD3D->SetBlendMode(D3D::BM_AlphaBlend);
	//gD3D->BindRenderTarget(renderTargetSceneMotionBlurred, NULL);
	//effect.SetVariableValue(varIndexBufferSceneColor, (ID3D10ShaderResourceView*)renderTargetSceneSingle->GetShaderResourceView());
	//passMotionBlurBlend.Apply();
	//gD3D->DrawScreenQuad();
	//gD3D->SetBlendMode(D3D::BM_None);


	RenderTarget* gatherSource;

	// Apply user post effect
	if (userPostEffect != NULL)
	{
		gD3D->BindRenderTarget(renderTargetSceneUserPostEffect, NULL);
		userPostEffect->SetSceneBuffer(renderTargetSceneSingle->GetShaderResourceView());

		userPostEffect->Bind(NULL);
		gD3D->DrawScreenQuad();


		gatherSource = renderTargetSceneUserPostEffect;
	}
	else
	{
		gatherSource = renderTargetSceneSingle;
	}


	// Gather DOF
	gD3D->BindRenderTarget(renderTargetFilter[0], NULL);
	effect.SetVariableValue(varIndexBufferSceneColor, gatherSource->GetShaderResourceView());
	passDOFGather.Apply();
	gD3D->DrawScreenQuad();

	//// Gather bloom
	//gD3D->BindRenderTarget(renderTargetFilter[0], NULL);
	//effect.SetVariableValue(varIndexBufferSceneColor, bloomGatherSource->GetShaderResourceView());
	//passBloomGather.Apply();	
	//gD3D->DrawScreenQuad();

	// Blur pass 1
	gD3D->BindRenderTarget(renderTargetFilter[1], NULL);
	effect.SetVariableValue(varIndexBufferFilter, (ID3D10ShaderResourceView*)renderTargetFilter[0]->GetShaderResourceView());
	effect.SetVariableValue(varIndexBlurPixelDir, bloomBlurDirH);
	passBlur.Apply();
	gD3D->DrawScreenQuad();

	// Blur pass 2
	gD3D->BindRenderTarget(renderTargetFilter[0], NULL);
	effect.SetVariableValue(varIndexBufferFilter, (ID3D10ShaderResourceView*)renderTargetFilter[1]->GetShaderResourceView());
	effect.SetVariableValue(varIndexBlurPixelDir, bloomBlurDirV);
	passBlur.Apply();	
	gD3D->DrawScreenQuad();

	// Combine bloom results
	const Vector2& visible_area = gD3D->GetRenderScale();
	effect.SetVariableValue(varIndexRenderScale, visible_area);
	effect.SetVariableValue(varIndexDOFDepthNear, dofSettings.depthNear);
	effect.SetVariableValue(varIndexDOFDepthFocus, dofSettings.depthFocus);
	effect.SetVariableValue(varIndexDOFDepthFar, dofSettings.depthFar);

	gD3D->BindBackbuffer(NULL);
	gD3D->ClearBackBuffer();

	effect.SetVariableValue(varIndexBufferFilter, renderTargetFilter[0]->GetShaderResourceView());
	passCombine.Apply();	
	gD3D->DrawScreenQuad();


	// Reset bound variables again
	effect.SetVariableValue(varIndexBufferSceneColor, (ID3D10ShaderResourceView*)NULL);
	effect.SetVariableValue(varIndexBufferFilter, (ID3D10ShaderResourceView*)NULL);
	passCombine.Apply();
}

void PostProcess::InitBloomGatherSamples()
{
	int w, h;
	gD3D->GetViewportSize(&w, &h);

	Vector3 offsets[4];

	float x = 1.0f / (float)w;
	float y = 1.0f / (float)h;

	Vector4 packedOffsets[2];

	packedOffsets[0].x = -x;
	packedOffsets[0].y = -y;
	packedOffsets[0].w = -x;
	packedOffsets[0].z =  y;
	packedOffsets[1].x =  x;
	packedOffsets[1].y = -y;
	packedOffsets[1].w =  x;
	packedOffsets[1].z =  y;

	effect.SetVariableValue(varIndexGatherSamples, packedOffsets, 2);
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

	effect.SetVariableValue(varIndexBlurWeights, weights, POSTPROCESS_BLOOMBLUR_NUMSAMPLES);
	effect.SetVariableValue(varIndexBlurSamples, offsets, POSTPROCESS_BLOOMBLUR_NUMSAMPLES);


	// Set filter directions
	bloomBlurDirH.x = filterSizeInv.x;
	bloomBlurDirH.y = 0;
	bloomBlurDirV.x = 0;
	bloomBlurDirV.y = filterSizeInv.y;
}


void PostProcess::Clear()
{
	gD3D->ClearDepthStencil();
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
