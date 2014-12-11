
#include "../../../Src/Core/Shaders/MaterialConstants.inc"
#include "../../../Src/Core/Shaders/ScreenQuad.inc"
#include "../../../Src/Player/Content/Constants.inc"

struct PSOutput
{
	float4 color : SV_Target0;
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
