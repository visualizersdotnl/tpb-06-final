
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
	float4x4 viewInvMatrix; // required by code, although not used.
	float2 quadScaleFactor;
	float opacity = 1;
	float4 overlayProgress = 0; // x=overlay progress 0..1, y=overlay progress time since start, z=scene progress 0..1, w = 0
};


VSOutput MainVS(VSInput input)
{ 
	VSOutput output;
	output.normalizedPos = input.position.xy * float2(1,-1);
	output.screenPos = float4(input.position.xy * quadScaleFactor, 0, 1);

	//output.normalizedPos.y -= (1.0-opacity) * 0.45;
	
	return output;
}


Texture2D texture_testoverlay;

SamplerState samplerTexture
{
	AddressU = CLAMP;
	AddressV = CLAMP;
	Filter = MIN_MAG_LINEAR_MIP_POINT;
};




PSOutput MainPS(VSOutput input)
{
	PSOutput result;
	
	float2 p = input.normalizedPos * (0.5).xx + (0.5).xx;

	float2 uv = p * 1.0;

	result.color = texture_testoverlay.Sample(samplerTexture, uv);
	result.color.a *= opacity;

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
