
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
	float3 normal : TEXCOORD2;
	float3 view : TEXCOORD3;
};


cbuffer paramsOnlyOnce
{
	float2 renderScale; // How much of our screen we render. Used for limiting the vertical render range when using a different aspect ratio. (1,1 = whole screen)
	float4x4 viewProjMatrix;
	float4x4 projMat;
	float4x4 mWorld;
	float4x4 mWorldInv;
	float shininess;
	float overbright;
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
	float specular = overbright*pow(max(dot(normal, H), 0), shininess);

	return overbright*(lightColor*diffuse + specular);
}	

VSOutput MainVS(VSInput input)
{ 
	VSOutput output;

	float4 worldPos = mul(input.position, mWorld);
	output.screenPos = mul(worldPos, viewProjMatrix);

	float3 lightPos = float3(0.f, 0.f, 1.f);

	output.color = 0.f + 
		float4(LightVertex(
			input.position.xyz, 
			input.normal, 
			lightPos, 
			float3(1.f, 1.f, 1.f)), 1.f); // Let the env. map take care of color.

	float3 worldNormal = mul(input.normal, (float3x3) mWorld);
	output.texCoord = worldNormal.xy*0.5f + 0.5f;
	
	output.texCoordProj = worldPos.xy; // input.position.xy;

	output.normal = worldNormal;
	output.view = normalize(viewProjMatrix._41_42_43 - worldPos);

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
	float viewFacing = dot(normalize(input.view), normalize(input.normal));
	float rim = saturate((viewFacing - 0.2)*8.0);

	float2 uv = input.texCoord;
	float4 texColor = textureMap.Sample(samplerTexture, uv);
	float4 projColor = projMap.Sample(samplerTexture, input.texCoordProj);

	return texColor*projColor*input.color*float4(rim.xxx,1.0);
}



technique10 Blobs
{
	pass Default
	{
		SetVertexShader( CompileShader(vs_4_0, MainVS()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS()) );		
	}
}
