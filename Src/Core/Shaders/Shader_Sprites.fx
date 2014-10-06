
struct VSInput
{	
	float3 position : POSITION;
	float2 texCoord : TEXCOORD0;	
};

struct VSOutput
{
	float4 screenPos : SV_Position;
	float2 texCoord : TEXCOORD0;
};

struct PSOutput
{
	float4 color : SV_Target0;
};

cbuffer paramsOnlyOnce
{
	float2 renderScale; // How much of our screen we render. Used for limiting the vertical render range when using a different aspect ratio. (1,1 = whole screen)

	float2 position;
	float2 size;
};


VSOutput MainVS(VSInput input)
{ 
	VSOutput output;
	output.screenPos = float4( (input.position.xy * size + position) * renderScale, 0, 1);
	output.texCoord = (input.position.xy + float2(1,1))*0.5;
	return output;
}

	
Texture2D textureMap;

SamplerState samplerTexture
{
	AddressU = CLAMP;
	AddressV = CLAMP;
	Filter = MIN_MAG_MIP_LINEAR;
};



PSOutput MainPS(VSOutput input)
{
	PSOutput result;
	
	float2 uv = input.texCoord;

	result.color = textureMap.Sample(samplerTexture, uv);	
	
	return result;
}


technique10 Sprites
{
	pass Default
	{
		SetVertexShader( CompileShader(vs_4_0, MainVS()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS()) );		
	}
}

