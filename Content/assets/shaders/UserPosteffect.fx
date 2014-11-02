// Winner post effect

struct VSInput
{
	float3 position : POSITION;
};

struct VSOutput
{
	float2 normalizedPos : TEXCOORD0;
	float4 screenPos : SV_Position;
};



struct PSOutput
{
	float4 color : SV_Target0;
};


cbuffer paramsOnlyOnce
{
	// Required by code, although not used..
	float4x4 viewInvMatrix; 

	float sceneRenderLOD = 1;		

	// Scaling factor to render our full screen quad with a different aspect ratio (X=1, Y<=1)
	float2 quadScaleFactor;

	float opacity = 1;

	// These are bound to a Rocket tracks.
	float g_preSpriteFade; // 0 = normal, 1 = white, -100 = real black!
	float g_sceneNoise; // 0 = none, 1 = full (before fade)
	float g_sceneNoiseT;
};


VSOutput MainVS(VSInput input)
{ 
	VSOutput output;

	output.normalizedPos = input.position.xy;
	output.screenPos = float4(input.position.xy * quadScaleFactor, 0, 1);
	return output;
}

Texture2D sceneBuffer;

SamplerState samplerSceneBuffer
{
	AddressU = CLAMP;
	AddressV = CLAMP;
	Filter = MIN_MAG_LINEAR_MIP_POINT;
};

// Noise function
float rand(float2 co){
    return frac(sin(dot(co.xy ,float2(12.9898,78.233))) * 43758.5453);
}

PSOutput MainPS(VSOutput input)
{
	PSOutput result;


	// Screen uv in [0,1] for both x and y
	float2 screenUV = input.normalizedPos*float2(0.5, -0.5) + (0.5).xx;

	result.color = sceneBuffer.SampleLevel(samplerSceneBuffer, screenUV, 0);
	result.color.xyz += rand(screenUV+g_sceneNoiseT.xx)*g_sceneNoise;
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
