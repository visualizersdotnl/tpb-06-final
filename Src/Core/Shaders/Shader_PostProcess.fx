
#include "ScreenQuad.inc"

// Since the target filter RT is fixed-point, we have to rescale all colors back
// to some "allowed" range.
#define MAX_FILTER_COLOR 4.f

// Default is/was 15 (keep in check with C++).
#define NUM_BLOOMBLUR_SAMPLES 15


struct PSOutput
{
	float4 color : SV_Target0;
};

cbuffer PostProcessConstants
{
	// Bloom parameters.
	float4 bloomGatherSamples[2];
	float4 bloomBlurSamples[NUM_BLOOMBLUR_SAMPLES];
	float bloomBlurWeights[NUM_BLOOMBLUR_SAMPLES];
	float2 bloomBlurPixelDir;

	// Inverse filter buffer size.
	float2 filterSizeInv; 
	
	// Weight of current frame.
	float motionBlurFrameWeight; 

	// Loading bar progress (drawn if > 0).
	float loadProgress = 0.f;	
};
	
Texture2D bufferSceneColor;
Texture2D bufferFilter;

SamplerState samplerSceneColor
{
	AddressU = CLAMP;
	AddressV = CLAMP;
	Filter = MIN_MAG_MIP_LINEAR;
};

SamplerState samplerFilter
{
	AddressU = CLAMP;
	AddressV = CLAMP;
	Filter = MIN_MAG_MIP_LINEAR;
};

PSOutput MainPS_Gather(VSOutput input)
{
	PSOutput result;

	float2 uv = input.screenPos.xy * filterSizeInv;

	float3 averageBloom = 0; 

	for (int i=0; i<2; ++i)
	{
		float4 sceneColor0 = bufferSceneColor.Sample(samplerSceneColor, uv+bloomGatherSamples[i].xy);
		averageBloom += any(sceneColor0.rgb > 1) ? sceneColor0.rgb : 0;

		float4 sceneColor1 = bufferSceneColor.Sample(samplerSceneColor, uv+bloomGatherSamples[i].wz);
		averageBloom += any(sceneColor1.rgb > 1) ? sceneColor1.rgb : 0;
	}

	averageBloom = averageBloom / 4;

	result.color = float4(averageBloom, 0) / MAX_FILTER_COLOR;	
	return result;
}

PSOutput MainPS_Blur(VSOutput input)
{
	PSOutput result;

	float2 uv = input.screenPos.xy * filterSizeInv;

	float4 totalSum = 0;

	for (int i=0; i<NUM_BLOOMBLUR_SAMPLES; i++)
	{
		totalSum += bufferFilter.Sample(samplerFilter, uv+bloomBlurSamples[i].xy*bloomBlurPixelDir) * bloomBlurWeights[i];
	}
	
	result.color = totalSum;
	
	return result;
}

PSOutput MainPS_Combine(VSOutput input)
{
	PSOutput result;

	float2 uv = input.uv;

	float4 color = bufferSceneColor.Sample(samplerSceneColor, uv);
	color += bufferFilter.Sample(samplerFilter, uv) * MAX_FILTER_COLOR;	
	result.color = color;
	
	// The Inque loading bar.
	// FIXME: move to user post-proc. shader.
	const float _loadProgress = loadProgress;
	if (_loadProgress > 0)
	{
		if (uv.y >= 0.40 && uv.y <= 0.60 && uv.x >= 0.08 && uv.x <= 0.92)
		{
			float v;

			if (uv.x <= 0.09 || uv.x >= 0.91 || uv.y <= 0.418 || uv.y >= 0.582)
				v = 1.0;
			else if (uv.y >= 0.436 && uv.y <= 0.564 && 
				uv.x >= 0.10 && uv.x < 0.10+0.8*_loadProgress)
			{
				float k = (1.0-uv.x);
				float r = sin(k * 120.0);
				v = (r > (0.8-_loadProgress)) ? 1.0 : 0.0;
			}
			else 
				v = 0.0;

			result.color += float4(v,v,v,1);	
		}
	}
	
	return result;
}

PSOutput MainPS_MotionBlur(VSOutput input)
{
	PSOutput result;
	
	float2 uv = input.uv; // No LOD.
	float4 sceneColor = bufferSceneColor.Sample(samplerSceneColor, uv);
	result.color = float4(sceneColor.rgb, motionBlurFrameWeight);	

	return result;
}


technique11 PostFX
{
	pass Gather
	{
		SetVertexShader( CompileShader(vs_4_0, ScreenQuadVS()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS_Gather()) );		
	}
	
	pass Blur
	{
		SetVertexShader( CompileShader(vs_4_0, ScreenQuadVS()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS_Blur()) );
	}	
	
	pass Combine
	{
		SetVertexShader( CompileShader(vs_4_0, ScreenQuadVS()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS_Combine()) );
	}	

	pass MotionBlur
	{
		SetVertexShader( CompileShader(vs_4_0, ScreenQuadVS()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS_MotionBlur()) );
	}
}
