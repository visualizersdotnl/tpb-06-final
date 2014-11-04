
// Since the target filter RT is fixed-point, we have to rescale all colors back
// to some "allowed" range.
#define MAX_FILTER_COLOR 4.0f


#define NUM_BLOOMBLUR_SAMPLES 15


struct VSInput
{
	float3 position : POSITION;
};

struct VSOutput
{
	float4 screenPos : SV_Position;
};

struct PSOutput
{
	float4 color : SV_Target0;
};

cbuffer paramsOnlyOnce
{
	float4 bloomGatherSamples[2];
	float4 bloomBlurSamples[NUM_BLOOMBLUR_SAMPLES];
	float bloomBlurWeights[NUM_BLOOMBLUR_SAMPLES];
	float2 bloomBlurPixelDir;

	float4 screenSizeInv; // conversion scale (XY) and offset (ZW) to convert from screen buffer's pixels to "rendered" screen buffer texels (takes sceneRenderLOD into account)
	float2 filterSizeInv; // conversion scale (XY) to convert from filter buffer's pixels to filter buffer's texels. (just the inv of the size)
	float4 filterPixelsToScreenUV; 
	float2 renderScale; // How much of our screen we render. Used for limiting the vertical render range when using a different aspect ratio. (1,1 = whole screen)
	
	float motionBlurFrameWeight; // weight of current frame

	float loadProgress = 0;	
};

Texture2D loadingTexture; // 1920*1080 16:9

VSOutput MainVS(VSInput input)
{ 
	VSOutput output;
	output.screenPos = float4(input.position.xy * renderScale, 0, 1);
	return output;
}

VSOutput MainVS_Combine(VSInput input)
{ 
	VSOutput output;

	output.screenPos = float4(input.position.xy * renderScale, 0, 1);
	return output;
}

	
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
	
	float2 uv = input.screenPos.xy * screenSizeInv.xy + screenSizeInv.zw;

	float4 color = bufferSceneColor.Sample(samplerSceneColor, uv);
	color += bufferFilter.Sample(samplerFilter, uv) * MAX_FILTER_COLOR;	
	

	
	result.color = color;	

	if (loadProgress > 0)
	{
		float2 loadUV = uv;
		result.color = loadingTexture.SampleLevel(samplerSceneColor, loadUV, 0);
	}
	
	if (loadProgress > 0 && 
		uv.y >= 0.40 && uv.y <= 0.60 && 
		uv.x >= 0.08 && uv.x <= 0.92)
	{
		float v;

		if (uv.x <= 0.09 || uv.x >= 0.91 || uv.y <= 0.418 || uv.y >= 0.582)
			v = 1.0;
		else if (uv.y >= 0.436 && uv.y <= 0.564 && 
			uv.x >= 0.10 && uv.x < 0.10+0.8*loadProgress)
		{
			float k = (1.0-uv.x);
			float r = sin(k * 120.0);
			v = (r > (0.8-loadProgress)) ? 1.0 : 0.0;
		}
		else 
			v = 0.0;

		// @plek: blend into Bond background
		v *= 1.f-uv.x;

		result.color += float4(v,v,v,1);	
	}
	
	return result;
}


PSOutput MainPS_MotionBlur(VSOutput input)
{
	PSOutput result;
	
	// Remove our sceneRenderLod factor from the screensize; don't take that into account here
	float sceneRenderLod = 1-(screenSizeInv.z*2);
	float2 uv = input.screenPos.xy * (screenSizeInv.xy/sceneRenderLod);

	float4 sceneColor = bufferSceneColor.Sample(samplerSceneColor, uv);

	result.color = float4(sceneColor.rgb, motionBlurFrameWeight);	
	return result;
}


technique10 PostFX
{
	pass Gather
	{
		SetVertexShader( CompileShader(vs_4_0, MainVS()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS_Gather()) );		
	}
	
	pass Blur
	{
		SetVertexShader( CompileShader(vs_4_0, MainVS()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS_Blur()) );
	}	
	
	pass Combine
	{
		SetVertexShader( CompileShader(vs_4_0, MainVS_Combine()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS_Combine()) );
	}	

	pass MotionBlur
	{
		SetVertexShader( CompileShader(vs_4_0, MainVS()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS_MotionBlur()) );
	}
}

