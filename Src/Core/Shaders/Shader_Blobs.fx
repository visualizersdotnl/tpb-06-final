
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
	float2 texCoordProj : TEXCOORD1;
};


cbuffer paramsOnlyOnce
{
	float2 renderScale; // How much of our screen we render. Used for limiting the vertical render range when using a different aspect ratio. (1,1 = whole screen)
	float4x4 viewProjMatrix;
	float4x4 projMat;
	float4x4 mWorld;
	float4x4 mWorldInv;
};

float3 LightVertex(
	float3 position,
	float3 normal,
	float3 lightPos,
	float3 lightColor)
{
	lightPos = mul(lightPos, (float3x3) mWorldInv);

	float3 L = normalize(lightPos - position);
	float diffuse = max(dot(normal, L), 0.f);

	float3 eyePos = lightPos; // Not ideal, but it'll do the trick.
	float3 V = normalize(eyePos - position);
	float3 H = normalize(L + V);
	float specular = 2.f*pow(max(dot(normal, H), 0), 9.f);

	return lightColor*diffuse + specular;
}	

VSOutput MainVS(VSInput input)
{ 
	VSOutput output;

	float4 worldPos = mul(input.position, mWorld);
	output.screenPos = mul(worldPos, viewProjMatrix);

	output.color = 0.f + 
		float4(LightVertex(
			input.position.xyz, 
			input.normal, 
			float3(0.f, 0.f, 1.f), 
			1.5f*float3(1.f, 1.f, 1.f)), 1.f); // Let the env. map take care of color.
			// ^ But still put it in overdrive to hit the bloom if we can.
//			1.2f*float3(47.f/255.f, 156.f/255.f, 152.f/255.f)), 1.f);

	float3 worldNormal = mul(input.normal, (float3x3) mWorld);
	output.texCoord = worldNormal.xy*0.5f + 0.5f;
	
	output.texCoordProj = worldPos.xy; // input.position.xy;

	return output;
}

	
Texture2D textureMap;
Texture2D projMap;

SamplerState samplerTexture
{
	AddressU = WRAP;
	AddressV = WRAP;
	Filter = MIN_MAG_MIP_LINEAR;
};



float4 MainPS(VSOutput input) : SV_Target0
{
	float2 uv = input.texCoord;
	float4 texColor = textureMap.Sample(samplerTexture, uv);
//	return texColor*input.color;
	float4 projColor = projMap.Sample(samplerTexture, input.texCoordProj);
//	return projColor;
	return texColor*projColor*input.color;
}



technique10 Blobs
{
	pass Default
	{
		SetVertexShader( CompileShader(vs_4_0, MainVS()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS()) );		
	}
}
