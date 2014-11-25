
struct VSInput
{
	float3 position : POSITION;
};

struct VSOutput
{
	float4 screenPos : SV_Position;
	float2 uv : TEXCOORD0;
};

struct PSOutput
{
	float4 color : SV_Target0;
};

// #include "../../../Src/Core/Shaders/MatConsts.inc"

cbuffer paramsOnlyOnce
{
	// Required by Material.
	float4x4 viewInvMatrix; 

	// These are bound to a Rocket tracks.
	float g_preSpriteFade; // 0 = 100%, 1 = white, -100 = completely black.
	float g_sceneNoise;    // 0 = none, 1 = opaque.
	float g_sceneNoiseT;
};


VSOutput MainVS(VSInput input)
{ 
	VSOutput output;

	output.screenPos = float4(input.position.xy, 0, 1);
	output.uv = input.position.xy * float2(0.5f, -0.5f) + 0.5f;

	return output;
}


// Supplied by Material instance.
Texture2D sceneBuffer;

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


technique10 Default
{
	pass Default
	{
		SetVertexShader( CompileShader(vs_4_0, MainVS()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS()) );		
	}
}
