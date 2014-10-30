
struct VSInput
{	
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct VSOutput
{
	float4 screenPos : SV_Position;
	float4 color : COLOR;
	float2 texCoord : TEXCOORD0;
};


cbuffer paramsOnlyOnce
{
	float2 renderScale; // How much of our screen we render. Used for limiting the vertical render range when using a different aspect ratio. (1,1 = whole screen)
	float4x4 viewProjMatrix;
};

float3 VL_CalculateDiffuseTerm(
	float3 position,
	float3 normal,
	float3 lightPos,
	float3 lightColor)
{
//	FIXME: relevant when using a world trans. (inverse transform)
//	lightPos = mul(lightPos, mCustom);

	float3 lightVec = normalize(lightPos - position);
	float diffuse = max(dot(normal, lightVec), 0);
	float3 diffuseTerm = diffuse * lightColor;
	return diffuseTerm;
}	

VSOutput MainVS(VSInput input)
{ 
	VSOutput output;
	output.screenPos = mul(float4(input.position, 1.f), viewProjMatrix);
	float3 rotNormal = input.normal; // FIXME: transform when using a world trans.
	output.color = 0.25f + float4(VL_CalculateDiffuseTerm(input.position, rotNormal, float3(0.f, 0.f, 1.f), float3(1.f, 1.f, 1.f)), 1.f);
	output.texCoord = rotNormal.xy*0.5f + 0.5f;
	return output;
}

	
Texture2D textureMap;

SamplerState samplerTexture
{
	AddressU = CLAMP;
	AddressV = CLAMP;
	Filter = MIN_MAG_MIP_LINEAR;
};



float4 MainPS(VSOutput input) : SV_Target0
{
	float2 uv = input.texCoord;
	float4 texColor = textureMap.Sample(samplerTexture, uv);	
	return texColor*input.color;
}


technique10 Blobs
{
	pass Default
	{
		SetVertexShader( CompileShader(vs_4_0, MainVS()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS()) );		
	}
}