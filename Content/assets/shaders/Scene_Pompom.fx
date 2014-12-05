
#include "../../../Src/Core/Shaders/MaterialConstants.inc"
#include "../../../Src/Core/Shaders/ScreenQuad.inc"

 
struct PSOutput
{
	float4 color : SV_Target0;
};

cbuffer Constants
{
	float g_fxTime;

	float4x4 pompomRotMat;
	float4x4 pompomRotMatInv;
};

SamplerState samplerTexture
{
	AddressU = WRAP;
	AddressV = WRAP;
	Filter = MIN_MAG_MIP_LINEAR;
};

Texture2D texture_pompom_noise;	
Texture2D texture_pompom_color;

// --------------------------------------------------------------------------------------------

static const float uvScale = 1;
static const float colorUvScale = 1.0;
static const float furDepth = 0.3;
static const int furLayers = 128;//96;
static const float rayStep = furDepth*2.0 / float(furLayers);
static const float furThreshold = 0.3;
static const float shininess = 50.0;

bool intersectSphere(float3 ro, float3 rd, float r, out float t)
{
	float b = dot(-ro, rd);
	float det = b*b - dot(ro, ro) + r*r;
	if (det < 0.0) return false;
	det = sqrt(det);
	t = b - det;
	return t > 0.0;
}

float3 rotateX(float3 p, float a)
{
    float sa = sin(a);
    float ca = cos(a);
    return float3(p.x, ca*p.y - sa*p.z, sa*p.y + ca*p.z);
}

/*
float3 rotateY(float3 p, float a)
{
    float sa = sin(a);
    float ca = cos(a);
    return float3(ca*p.x + sa*p.z, p.y, -sa*p.x + ca*p.z);
}
*/

float2 cartesianToSpherical(float3 p)
{		
	float r = length(p);

	float t = (r - (1.0 - furDepth)) / furDepth;	
	p = rotateX(p.zyx, -cos(g_fxTime)*t*t*0.5).zyx;	// curl

	p /= r;	
	float2 uv = float2(atan2(p.y, p.x), acos(p.z));

	//uv.x += cos(g_fxTime*1.5)*t*t*0.4;	// curl
	//uv.y += sin(g_fxTime*1.7)*t*t*0.2;

	uv.y -= t*t*0.25;	// curl down
	return uv;
}

// returns fur density at given position
float furDensity(float3 pos, out float2 uv)
{
	uv = cartesianToSpherical(pos.xzy);	
	float4 tex = texture_pompom_noise.SampleLevel(samplerTexture, uv*uvScale, 0); // texture2D(iChannel0, uv*uvScale);
	
	// thin out hair
	float density = smoothstep(furThreshold, 1.0, tex.x);
	
	float r = length(pos);
	float t = (r - (1.0 - furDepth)) / furDepth;
	
	// fade out along length
	float len = tex.y;
	density *= smoothstep(len, len-0.2, t);

	return density;	
}

// calculate normal from density
float3 furNormal(float3 pos, float density)
{
    float eps = 0.01;
    float3 n;
	float2 uv;
    n.x = furDensity( float3(pos.x+eps, pos.y, pos.z), uv ) - density;
    n.y = furDensity( float3(pos.x, pos.y+eps, pos.z), uv ) - density;
    n.z = furDensity( float3(pos.x, pos.y, pos.z+eps), uv ) - density;
    return normalize(n);
}

float3 furShade(float3 pos, float2 uv, float3 ro, float density)
{
	// lighting
//	const float3 L = mul(float3(0, 1, 0), (float3x3)pompomRotMatInv);
	const float3 L = float3(0, 1, 0);
	float3 V = normalize(ro - pos);
	float3 H = normalize(V + L);

	float3 N = -furNormal(pos, density);
	//float diff = max(0.0, dot(N, L));
	float diff = max(0.0, dot(N, L)*0.5+0.5);
	float spec = pow(max(0.0, dot(N, H)), shininess);
	
	// base color
	float3 color = texture_pompom_color.SampleLevel(samplerTexture, uv*colorUvScale, 0); 

	// darken with depth
	float r = length(pos);
	float t = (r - (1.0 - furDepth)) / furDepth;
	t = clamp(t, 0.0, 1.0);
	float i = t*0.5+0.5;

	float3 specCol = color*1.5;

	// crappy rim
	float viewFacing = dot(V, N);
	float rim = saturate((viewFacing - 0.2)*12.0);
	color *= rim;	
		

	return color*diff*i + float3(spec*specCol*i);
}		

float4 scene(float3 ro,float3 rd)
{
	float3 p = float3(0.0, 0.0, 0.0);
	const float r = 1.0;
	float t;				  
	bool hit = intersectSphere(ro - p, rd, r, t);
	
	float w = 1;
	float4 c = float4(0.0, 0.0, 0.0, 0.0);
	if (hit) {
		float3 pos = ro + rd*t;

		// ray-march into volume
		for(int i=0; i<furLayers; i++) {
			float4 sampleCol;
			float2 uv;
			sampleCol.a = furDensity(pos, uv);
			if (sampleCol.a > 0.0) {
				sampleCol.rgb = furShade(pos, uv, ro, sampleCol.a);

				// pre-multiply alpha
				sampleCol.rgb *= sampleCol.a * w;
				c = c + sampleCol*(1.0 - c.a);
				if (c.a > 0.95) break;
			}
			
			pos += rd*rayStep;
			w = max(0, w-0.007);
		}
	}
	
	float intensity = saturate((1-(pow(t, 6.45) * 0.02))*1.5);
	//c.rgb -= saturate(c.a.xxx)*0.01;
	//c.rgb = intensity.xxx;
	c.a = intensity;

	return c;
}

float4 ripped_main(float4 rayDir)
{
	float2 uv = float2(rayDir.x, rayDir.y);
	
	float3 ro = float3(0.0, 0.0, 2.5);
	float3 rd = normalize(float3(uv, -2.0));
	
	ro = mul(ro, pompomRotMat);
	rd = mul(rd, pompomRotMat);

	return scene(ro, rd);
}

// --------------------------------------------------------------------------------------------


PSOutput MainPS(VSOutput_RM input)
{
	PSOutput result;

    result.color = ripped_main(input.rayDir);

	return result;
}

technique11 Default
{
	pass Default
	{
		SetVertexShader( CompileShader(vs_4_0, ScreenQuadVS_RM()) );
		SetPixelShader( CompileShader(ps_4_0, MainPS()) );		
	}
}
