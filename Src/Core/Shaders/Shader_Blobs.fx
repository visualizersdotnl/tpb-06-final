
struct VSInput
{	
	float4 position : POSITION;
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
	float4x4 mWorld;
	float4x4 mWorldInv;
};

float3 VL_CalculateDiffuseTerm(
	float3 position,
	float3 normal,
	float3 lightPos,
	float3 lightColor)
{
	lightPos = mul(lightPos, (float3x3) mWorldInv);
	float3 lightVec = normalize(lightPos - position);
	float diffuse = max(dot(normal, lightVec), 0);
	float3 diffuseTerm = diffuse * lightColor;
	return diffuseTerm;
}	

VSOutput MainVS(VSInput input)
{ 
	VSOutput output;

	float4 worldPos = mul(input.position, mWorld);
	output.screenPos = mul(worldPos, viewProjMatrix);
	output.color = 0.25f + float4(VL_CalculateDiffuseTerm(input.position.xyz, input.normal, float3(0.f, 0.f, 1.f), float3(1.f, 1.f, 1.f)), 1.f);
	float3 worldNormal = mul(input.normal, (float3x3) mWorld);
	output.texCoord = worldNormal.xy*0.5f + 0.5f;

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
