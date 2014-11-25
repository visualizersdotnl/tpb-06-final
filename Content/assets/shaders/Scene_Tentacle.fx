
#include "../../../Src/Core/Shaders/MatConsts.inc"


#define SHOW_NORMALS 0


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


// Maximum number of raymarch steps
static int MaxRaySteps = 110; //228;

// Maximum number of raymarch steps for Iq's soft shadow
static int MaxSoftShadowIqSteps = 80;

// Affects the minimum distance at which we consider a sample to be an intersection or not
static float MinimumDistance = pow(10.0, -2.6);

// Maximum distance to trace.
static float MaximumDistance = 200.0;

// Fraction of the actually calculated distance that we'll travel
static float StepFraction = 0.7;


cbuffer paramsFX
{
	float FOV = 0.7f;

	float fisheyeStrength = 6; //6;
	float fisheyeFOV = 2.0; //1.0;
	
	float fadeAmount = 1; // 0 = black, 1 = visible, 10 = white-ish

	float4 testLightPos = float4(20.0, 20.0, 20.0, 1.0);

	float g_fxTime = 0;	

	float ribbonsSpeed = 0.5; //< How fast do our ribbons wave?
	float ribbonsAppear = 1.0; //< [0..1] 0=invisible, 1=visible
	float ribbonsPhase = 0; //< Extra value added to wave time.
	float ribbonsWonkyness = 0; //< [0..1] The higher, the more they'll wiggle with depth.

	float foldOffsetZ = 0;
	float4x4 foldXformInv;
};




VSOutput MainVS(VSInput input)
{ 
	VSOutput output;

	output.screenPos = float4(input.position.xy * float2(1,1.8) * sceneRenderLOD, 0, 1);
	output.rayDir = float4(input.position.xy, 0.0, 0.0);
	
	return output;
}


Texture2D texture_ribbons_mesh;
Texture2D texture_ribbons_wall;
Texture2D texture_even_lachen;
Texture2D texture_tentacles_pattern;
Texture2D texture_tentacles_noise;

SamplerState samplerTexture
{
	AddressU = WRAP;
	AddressV = WRAP;
	Filter = MIN_MAG_MIP_LINEAR;
};





// Sigmoid smoothing
float4 DistSmooth(float4 a, float4 b)
{
	float k = 0.5;
	
	float4 d = b-a;
	

	{
		// at d>=k --> return a
		// at d=0 --> return (a+b)/2 ?
		// at d<=-k --> return b;
		
		float4 alpha = clamp(d/(-k), (-1).xxxx, (1).xxxx);	// -1 = a, 0=center, +1=b
		
		float4 y = 0.5*alpha*alpha*(3.0 - 2.0*abs(alpha));
		
		alpha = (0.5).xxxx + sign(alpha)*y; // bidirectional s-curve
		
		return lerp(a,b, alpha);
	}
}


float SmoothMax(float a, float b)
{
	float k = 0.09;

	float d = b-a;
	
	// if a is much smaller than b, take b
	if (d >= k)
		return b;
	// if b is much smaller than a, take a
	else if (d <= -k)
		return a;
	// otherwise blend a little bit
	else
	{
		// at d=k --> return b
		// at d=0 --> return (a+b)/2 ?
		// at d=-k --> return a;
		
		float alpha = (d/(-k))*0.5 + 0.5;
		alpha *= alpha*alpha;
		
		return lerp(b,a, alpha);
	}
}


float SafeMod(float x, float v)
{
	if (x > 0)
		return x % v;
	else
		return v - ((-x) % v);
}




float MinWithUV(float inA, float inB, float2 inA_UV, float2 inB_UV, out float2 outUV)
{
	if (inA < inB)
	{
		outUV = inA_UV;
		return inA;
	}
	else
	{
		outUV = inB_UV;
		return inB;
	}
}



static float spiralRadius = 0.2;

float DistanceToSpiral(float3 Pos)
{
	//Pos.y += 0.55;
	
	float r = 0.1;//0.2;

//float off = foldOffsetZ*12.0;


	// pos.z = 1 --> poo = 1
	// pos.z = 3 --> poo = 0

	float poo = 1;//(1.0 - (Pos.z-1.0)/(3.0-1.0));

	float wobbleSpeed = 1.2;
	float wobbleSpeed2 = 1.5;

	float k1 = wobbleSpeed + 0.1*sin((Pos.z)*0.2);
	float k2 = wobbleSpeed + 0.1*cos((Pos.z)*0.2);

	k1*=poo;
	k2*=poo;

//	k1 *= saturate(1.0 + Pos.z*0.4);
//	k2 *= saturate(1.0 + Pos.z*0.4);

	float2 p = Pos.xy + float2(r*sin((Pos.z-g_fxTime*wobbleSpeed2) * k1), r*cos( (Pos.z-g_fxTime*wobbleSpeed2) * k2));
	
	
	
#if 0	
	// cubes:
	float2 di = abs(p.xy) - spiralRadius.xx;
	float mc = max(di.x, di.y);
	float dCubic = min(mc, length(max(di, 0.0)));

	return dCubic;
	// spheres
	//float dSpheric = length(p.xy) - spiralRadius;

	//float weight = min(1.0, length(Pos)*0.4);
	
	//return lerp(dSpheric, dCubic, weight);
#else

	float r2 = length(p.xy);
	float dist = r2 - spiralRadius;

	//float2 texcoord = float2((Pos.z-g_fxTime)*0.2, atan2(p.x, p.y)*0.1);
	//dist -= (texture_1200302908.SampleLevel(samplerDisplaceTexture, texcoord.xy, 0).x-0.5) * 0.15;

	return dist;
#endif
}


static float phi = 1.61803399; // golden ratio.
static float3 n1 = normalize(float3(-1,phi-1,1/phi-1));
static float3 n2 = normalize(float3(phi-1,1/phi-1,-1));
static float3 n3 = normalize(float3(1/phi-1,-1, phi-1 )); 	   // horizontal axis, normal points to bottom
static float scale = phi*phi;
static float3 offset = float3(1,1,1);


// Based on:
// http://www.fractalforums.com/ifs-iterated-function-systems/kaleidoscopic-%28escape-time-ifs%29/105/
float DistanceToFoldedSpiral(float3 p)
{
	p = abs(p);
	
	float type = 0.0f;

	float t1 = dot(p, n1);
	if (t1 > 0) { p -= 2.0*t1*n1; type+=1.0; }
	float t2 = dot(p, n2);
	if (t2 > 0) { p -= 2.0*t2*n2; type+=1.0; }	
	float t3 = dot(p, n3);
	if (t3 > 0) { p -= 2.0*t3*n3; type+=1.0; }	
	
	
	p = mul(float4(p,1), foldXformInv).xyz;

	
	float rotAngle = g_fxTime + p.z * 0.4;
	
	// Extra rotate around Z axis
	float2 sc;
	sc.x = sin(rotAngle);
	sc.y = -sc.x;
	
	p.xy = p.xy * cos(rotAngle) + p.yx * sc.yx;


	//return DistanceToSpiral(p);
#if 0
	float cutSpeed = 1.0 + 0.5*sin(type*11.0); 
	
	p.z += g_fxTime;
	
	float twistedZ = p.z; //dot(p, viewInvMatrix._11_21_31);
	//p.z*0.7+p.x*0.7;
	
	// cut outs along z
	float dCutOut1 = (frac(twistedZ*cutSpeed + 0.6)) - 0.5;
	float cutter1 = abs(dCutOut1) - 0.07;

	float dCutOut2 = (frac(twistedZ*cutSpeed*0.87 + 0.2)) - 0.5;
	float cutter2 = abs(dCutOut2) - 0.04;
	
	
	return max(-cutter1, max(-cutter2, DistanceToSpiral(p)));
#else
	return DistanceToSpiral(p);
#endif
}


float DistToCenterSphere(float3 p)
{
	float r = 1.2 + 0.1*sin(g_fxTime);
	return length(p) - r;
}

float DistanceEstimator(float3 Pos, out float HitMat, out float2 outUV) 
{
	// float k = 15.0;
	// Pos.z = SafeMod(Pos.z + 0.5*k, k) - 0.5*k;



	outUV = float2(0,0);

	HitMat = 0;
	float d= DistSmooth(DistanceToFoldedSpiral(Pos), DistToCenterSphere(Pos));

	float2 uv = float2(atan2(Pos.x, Pos.y)*0.3, atan2(Pos.y, Pos.z)*0.4);
	d += 0.1*texture_tentacles_noise.SampleLevel(samplerTexture, uv.xy, 0).x;


	return d;

	// float2 uv0, uv1;

	// float d = DistToRibbonOuter(Pos,0,uv0);
	// d = MinWithUV(d, DistToRibbonOuter(Pos,3.7,uv1), uv0, uv1, outUV);
	// d = MinWithUV(d, DistToRibbonOuter(Pos,4.1,uv1), outUV, uv1, outUV);	
	// d = MinWithUV(d, DistToRibboInner(Pos, 0, uv1), outUV, uv1, outUV);
	// d = MinWithUV(d, DistToRibboInner(Pos,2.7,uv1), outUV, uv1, outUV);
	// d = MinWithUV(d, DistToRibboInner(Pos,3.9,uv1), outUV, uv1, outUV);

	// float dWall = Pos.z + 3.0;

	// if (d < dWall)
	// {
	// 	HitMat = 0;
	// 	return d;
	// }
	// else
	// {
	// 	HitMat = 1;
	// 	outUV = Pos.xy * 0.1;		
	// 	return dWall;
	// }
}


bool Trace(float4 From, float4 Dir, out float4 HitPos, out float HitMaterial, out float2 outUV)
{
	float dist = 0.0;
	float totalDist = 0.0;
	float stepEpsilon = 0.0;

	float4 pos = From;
	int step = 0;
	HitMaterial = 0;

	[fastopt] while (step < MaxRaySteps)
	{
		dist = DistanceEstimator(pos.xyz, HitMaterial, outUV);
		
		stepEpsilon = totalDist * MinimumDistance;
		if (dist < stepEpsilon)
			break;
		
		dist *= StepFraction;
			
		totalDist += dist;
			
		if (totalDist > MaximumDistance)
			break;			
		
		pos += dist * Dir;
		
		step++;
	}
	
	if (dist < stepEpsilon)
	{
		HitPos = pos;
		return true;
	}
	else
	{
		return false;
	}		
}


float3 Normal(float3 Pos)
{
	float3 eps = float3(0.0, 10e-4, 0.0);
	float dummy = 0;
	float2 dummyUV;
	
	float3 n = float3(
		DistanceEstimator(Pos+eps.yxx, dummy, dummyUV) - DistanceEstimator(Pos-eps.yxx, dummy, dummyUV),
		DistanceEstimator(Pos+eps.xyx, dummy, dummyUV) - DistanceEstimator(Pos-eps.xyx, dummy, dummyUV),
		DistanceEstimator(Pos+eps.xxy, dummy, dummyUV) - DistanceEstimator(Pos-eps.xxy, dummy, dummyUV));
	
	return normalize(n);
}



// Iq's softshadow:
// http://www.iquilezles.org/www/articles/rmshadows/rmshadows.htm 
// http://www.pouet.net/topic.php?which=7931&page=6 
//
// inHardness: 2 = quite smooth, 128 = quite hard
float SoftShadowIq(float3 inPos, float3 inLightDir, float inMinT, float inMaxT, float inHardness)
{
  float res = 1.0;
  float t = inMinT;
  float dummy;
  float2 dummyUV;

  for (int step = 0; step < MaxSoftShadowIqSteps; step++)
  {
      float d = DistanceEstimator(inPos + inLightDir*t, dummy, dummyUV);
      if (d < 0.0001)
          return 0.0;
      res = min( res, inHardness * d/t );
      t += d * 0.25;
	  
	  if (t > inMaxT)
		break;
  }
  return res;
}



static float3 ColOrange = float3(255.0,83.0,13.0)/255.0;

static float3 RibbonDiffuseBack = 0.3 * float3(143,100,85)/255.0;

static float3 RibbonDiffuseFront = 0.9 * float3(196,156,121)/255.0;
static float3 RibbonAmbient = 0.1 * float3(86,44,68)/255.0;
static float3 RibbonSpecular = float3(248,239,222)/255.0;

static float3 OrangeLineAmbient = 0.4*float3(210,69,0)/255.0;
static float3 OrangeLineDiffuse = 0.7*float3(254,173,0)/255.0;

float4 Shade(float3 inPos, float3 inNormal, float3 inEyeDir, float3 inEyePos, float inMatIndex, float2 inUV)
{
	float3 lightOffset = (testLightPos.xyz - inPos);
	
	float lightDist = length(lightOffset);
	float3 lightDir = normalize(lightOffset);
	
	float diffuseTerm = dot(inNormal, lightDir);


	float3 diffColor;
	float3 specColor;
	float3 ambient;
	float specAmount;
	float3 maxLight;
	//float specPower

	if (inMatIndex == 0)
	{		
		// Ribbons
		
		float3 flatPos = inPos;// - inNormal*dot(inPos, inNormal);
		float2 uv = float2(atan2(inPos.x, inPos.y)*0.3, atan2(inPos.y, inPos.z)*0.4);
		//inPos.xz * 0.60;

		float3 lineMask = texture_tentacles_pattern.SampleLevel(samplerTexture, uv.xy, 0).xyz;
		float depthAmount = saturate(inPos.z / -0.3);
		diffColor = lerp(RibbonDiffuseFront, RibbonDiffuseBack, depthAmount) * lineMask;
		ambient = RibbonAmbient;

		specColor = RibbonSpecular;
		specAmount = 0.2;

		float eps = sin((inPos.x - inPos.y)*12.0);
		float epsThreshold = 0.95;
		if (eps > epsThreshold)
		{
			eps = saturate(1.5*(eps - epsThreshold)/(1.0 - epsThreshold));
			ambient = lerp(ambient, OrangeLineAmbient, 0.1*eps);
			diffColor = lerp(diffColor, OrangeLineDiffuse, 0.1*eps);
		}		
	}
	else if (inMatIndex == 1)
	{
		// Wall

		float2 uv = inUV; //GetBlobbyUV(inPosLocal);
		float3 texel = float3(0,0,0);//texture_ribbons_wall.SampleLevel(samplerTexture, uv, 0).xyz;

		diffColor = texel * 0.6;
		ambient = texel*0.2;
		specAmount = 0.2;
		specColor = float3(0,0,0); //float3(1,0,1);
	}
	else
	{
		// Light
		ambient = float3(0.0, 1.0, 1.0) * 0.5;
		specAmount = 0;
		specColor = (0.0).xxx;
	}

	float3 diffuse = max(diffuseTerm, 0.0) * diffColor;
	
	float3 reflected = normalize((2.0f * diffuseTerm * inNormal) - lightDir);
	float specular = pow( max(0, dot(reflected, inEyeDir) ), 6 ) * specAmount;	
	
	float3 lightAmount = 
		diffuse + specular*specColor + ambient;

	float lightAttenuation = 1.0 / (1.0 + 0.1*dot(inPos,inPos));
	//1.0 / (1.0 + lightDist*lightDist*0.001 + lightDist*0.00002);	

	float shadowFactor = max(SoftShadowIq( inPos + inNormal*0.2, lightDir, 0.005, lightDist * 0.2, 6 ), 0.2);

	float3 col = lightAmount * lightAttenuation * shadowFactor * (pow(2.0, fadeAmount)-1.0);
	float opacity = saturate(lightAttenuation*1.2);
	return float4(col, opacity);
}


PSOutput MainPS(VSOutput input)
{
	PSOutput result;

	// Fisheye!
	float strength = 1.0 + pow(length(input.rayDir.xy), fisheyeStrength);// * 0.025;	
	float4 fishEyeScale = float4(strength.xx, 1.0, 0.0);	
	
    float4 rayDir = mul(input.rayDir*fishEyeScale, viewInvMatrix) * FOV * fisheyeFOV + -viewInvMatrix._31_32_33_34; // -forward vector


	float4 origin = viewInvMatrix._41_42_43_44;
	float4 dir = normalize(rayDir);
	
	float4 hitPos;
	float4 color;
	float hitMat = 0;
	float2 hitUV;


	if (Trace(origin, dir, hitPos, hitMat, hitUV))
	{
		float3 normal = Normal(hitPos.xyz);

		float depth = 1;//length(hitPos - origin);

#if SHOW_NORMALS		
		result.color = float4(normal.xyz*0.5 + (0.5).xxx,1);
#else
		result.color = Shade(hitPos.xyz, normal, -dir.xyz, origin, hitMat, hitUV);
#endif			
	}
	else
	{
		// Outside anything.
		result.color = float4(0,0,0,0);
	}


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
