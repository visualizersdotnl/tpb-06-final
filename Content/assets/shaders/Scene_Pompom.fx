
struct VSInput
{
	float3 position : POSITION;
};

struct VSOutput
{
	float4 screenPos	: SV_Position;
	float4 rayDir		: TEXCOORD0; // Worldspace view direction	
};
 
struct PSOutput
{
	float4 color : SV_Target0;
};

cbuffer paramsOnlyOnce
{
	float4x4 viewInvMatrix;
	float sceneRenderLOD = 1;	
	float2 quadScaleFactor;			// Scaling factor to render our full screen quad with a different aspect ratio (X=1, Y<=1)

	float g_fxTime;
};

VSOutput MainVS(VSInput input)
{ 
	VSOutput output;

	output.screenPos = float4(input.position.xy * quadScaleFactor * float2(1.f, 1.8f), 0, 1);
	output.rayDir = float4(input.position.xy, 1.0, 1.0);
	
	return output;
}

SamplerState samplerTexture
{
	AddressU = WRAP;
	AddressV = WRAP;
	Filter = MIN_MAG_MIP_LINEAR;
};

PSOutput MainPS(VSOutput input)
{
	PSOutput result;

	float4 screenPos = input.screenPos;
	result.color = float4(1.f, 1.f, 0.f, 1.f);

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
