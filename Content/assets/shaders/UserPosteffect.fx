
#include "../../../Src/Core/Shaders/MaterialConstants.inc"
#include "../../../Src/Core/Shaders/ScreenQuad.inc"

struct PSOutput
{
	float4 color : SV_Target0;
};

cbuffer Constants
{
	// These are bound to global Rocket tracks:
	float g_fxTime;
	float g_preSpriteFade; // 0 = Visible, 1 = White, -100 = Black
	float g_sceneNoise;    // 0 = None, 1 = Full
	float g_sceneNoiseT;
};

SamplerState samplerSceneBuffer
{
	AddressU = CLAMP;
	AddressV = CLAMP;
	Filter = MIN_MAG_LINEAR_MIP_POINT;
};

// Taken from a Shadertoy shader. Not that nice.
float rand(float2 co){
    return frac(sin(dot(co.xy ,float2(12.9898,78.233))) * 43758.5453);
}

PSOutput MainPS(VSOutput input)
{
	PSOutput result;

	// Sample scene buffer.
	result.color = sceneBuffer.SampleLevel(samplerSceneBuffer, input.uv, 0);

	// FIXME: this isn't exactly fantastic noise.	
	result.color.xyz = lerp(result.color.xyz, rand(input.uv+g_sceneNoiseT.xx).xxx, g_sceneNoise);

	// Scene fade (before the top-layer sprites are flushed).	
	result.color.xyz += g_preSpriteFade.xxx;

	return result;
}


technique11 Default
{
	pass Default
	{
		SetVertexShader( CompileShader(vs_4_0, ScreenQuadVS()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS()) );		
	}
}
