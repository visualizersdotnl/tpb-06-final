
struct VSInput
{	
	float3 position : POSITION;
	float4 color : COLOR;
	float2 texCoord : TEXCOORD0;	
};

struct VSOutput
{
	float4 screenPos : SV_Position;
	float4 color : COLOR;
	float2 texCoord : TEXCOORD0;
};


VSOutput MainVS(VSInput input)
{ 
	VSOutput output;
	output.screenPos = float4(input.position, 1.f);
	output.color = input.color;
	output.texCoord = input.texCoord;
	return output;
}


Texture2D textureMap;

SamplerState samplerTextureWrap
{
	AddressU = WRAP;
	AddressV = WRAP;
	Filter = MIN_MAG_MIP_LINEAR;
};

SamplerState samplerTextureClamp
{
	AddressU = CLAMP;
	AddressV = CLAMP;
	Filter = MIN_MAG_MIP_LINEAR;
};

float4 MainPS_Wrap(VSOutput input) : SV_Target0
{
	float2 uv = input.texCoord;
	float4 texColor = textureMap.Sample(samplerTextureWrap, uv);	
	return texColor*input.color;
}

float4 MainPS_Clamp(VSOutput input) : SV_Target0
{
	float2 uv = input.texCoord;
	float4 texColor = textureMap.Sample(samplerTextureClamp, uv);	
	return texColor*input.color;
}


technique10 Sprites
{
	pass Wrap
	{
		SetVertexShader( CompileShader(vs_4_0, MainVS()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS_Wrap()) );		
	}

	pass Clamp
	{
		SetVertexShader( CompileShader(vs_4_0, MainVS()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS_Clamp()) );		
	}
}
