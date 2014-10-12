
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


cbuffer paramsOnlyOnce
{
	float4x4 viewInvMatrix;
	float sceneRenderLOD = 1;	
	float2 quadScaleFactor;			// Scaling factor to render our full screen quad with a different aspect ratio (X=1, Y<=1)
	
	float FOV = 0.7f;

	float fisheyeStrength = 6; //6;
	float fisheyeFOV = 4.0; //1.0;
	
	float fadeAmount = 1; // 0 = black, 1 = visible, 10 = white-ish

	float4 testLightPos = float4(20.0, 20.0, 20.0, 1.0);

	float fxTimeGlobal = 0;	

	float knotCycle1 = 0;
	float knotCycle2 = 0;
	float knotCycle3 = 0;
	
	float knotGroupRadius = 0.4;
	float knotObjectRadius = 0.8;
	
	float knotTubeRadius1 = 0.01;
	float knotTubeRadius2 = 0.03;
	float knotTubeRadius3 = 0.05;

};




VSOutput MainVS(VSInput input)
{ 
	VSOutput output;


	output.screenPos = float4(input.position.xy * quadScaleFactor  * float2(1,1.8) * sceneRenderLOD, 0, 1);
//	output.rayDir = 
 //   	mul(float4(input.position.xy, 0.0, 0.0), viewInvMatrix) * FOV +
//		-viewInvMatrix._31_32_33_34; // -forward vector
	
	output.rayDir = float4(input.position.xy, 0.0, 0.0);
	
	return output;
}


Texture2D texture_ribbons_mesh;
Texture2D texture_ribbons_wall;
Texture2D texture_even_lachen;

SamplerState samplerTexture
{
	AddressU = WRAP;
	AddressV = WRAP;
	Filter = MIN_MAG_MIP_LINEAR;
};





float DistSmooth(float a, float b)
{
	float k = 0.1f;
	float d = b-a;
	
	// if a is much smaller than b, take a
	if (d >= k)
	{
		return a;
	}
	// if b is much smaller than a, take b
	else if (d <= -k)
	{
		return b;
	}
	// otherwise blend a little bit
	else
	{
		// at d=k --> return a
		// at d=0 --> return (a+b)/2 ?
		// at d=-k --> return b;
		
		float alpha = (d/(-k))*0.5 + 0.5;
		alpha *= alpha*alpha;

		
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


float DistToBoxSigned(float3 Pos, float3 Size)
{
	float3 di = abs(Pos) - Size;
	float mc = max(di.x, max(di.y, di.z));
	return min(mc,length(max(di, 0.0)));
}


float DistToSphere(float3 p, float r)
{
	return length(p.xyz) - r;
}



float DistToRibbon(float3 Pos, float inPhase, float2 inBoxSize, float inTwirlRadius, float inTwirlFreq, out float2 outUV)
{
	Pos.x += inTwirlRadius*cos(Pos.y*inTwirlFreq+inPhase + fxTimeGlobal);
	Pos.z += inTwirlRadius*sin(Pos.y*inTwirlFreq+inPhase + fxTimeGlobal);

	// Dist to signed box of infinite length in Y.
	float2 di = abs(Pos.xz) - inBoxSize;
	float mc = max(di.x, di.y);
	float d = min(mc, length(max(di, 0.0)));

	outUV = float2(Pos.y, Pos.x + 1.0) * 0.5;

	return d;
}

float DistToRibbonOuter(float3 Pos, float inPhase, out float2 outUV)
{
	float2 BoxSize = float2(0.2, 0.02);
	float TwirlRadius = 1.2+0.4*sin(Pos.y * 0.134 + fxTimeGlobal);
	
	return DistToRibbon(Pos, inPhase, BoxSize, TwirlRadius, 0.2, outUV);	
}


float DistToRibboInner(float3 Pos, float inPhase, out float2 outUV)
{
	float2 BoxSize = float2(0.15, 0.03);
	float TwirlRadius = 0.5+0.1*sin(Pos.y * 0.064 + 1.2 + fxTimeGlobal);
	
	return DistToRibbon(Pos, inPhase, BoxSize, TwirlRadius, 0.5, outUV);	
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



static float RotNumeratorX = -4.0;
static float RotNumeratorY = -4.0;
static float RotDenominator = 3.0;



float DistToPolarTwister(float3 Pos, float Phase, float TubeRadius, float Period)
{



	float ra = Pos.z * (RotNumeratorX/RotDenominator);
	float raz = Pos.z * (RotNumeratorY/RotDenominator);
	
	float2 polarTubeCenter = knotGroupRadius*float2(cos(ra), sin(raz)) + knotObjectRadius.xx;

	float2 polarOffset = Pos.xy - polarTubeCenter;
	float polarDist = length(polarOffset);
	
	float2 polarOffsetNorm = polarOffset / polarDist;
	float k = 0.5 + 1.0f / ( (abs(polarOffsetNorm.x) * abs(polarOffsetNorm.y)) + 1);
	
	k += 3.5*sin(ra*1.0 - fxTimeGlobal);
	
	float dist = polarDist - TubeRadius*k;
	
	// float distChop = abs( SafeMod(Pos.z, 1.1) - 1.0) - 0.1;
	float distChop = abs( SafeMod(Pos.z+Phase+fxTimeGlobal, Period) - 0.1) - 0.05;
	
	return max(-distChop, dist);
}


float DistToKnot(float3 Pos, float Phase, float Offset, float TubeRadius, float Period)
{
	// HACK to test syncing
	//TubeRadius *= pow(1.0-inqueAxisStretchY*0.1, 16.0);	
	
	float r = length(Pos.xz) + Offset;
	float angle = atan2(Pos.z, Pos.x);
	
	float3 polarPos = float3(r - knotObjectRadius, Pos.y, angle);
	
	float d = 1000.0;
	
	for (int i=0; i<3; ++i)
	{		
		d = min(d, DistToPolarTwister(polarPos, Phase, TubeRadius, Period));
		polarPos.z += 2.0*3.14159265358979;
	}
	
	return d;	
}




float DistanceEstimator(float3 Pos, out float HitMat, out float2 outUV)
{
	HitMat = 0;
	outUV = float2(0,0);

	//Pos.x = SafeMod(Pos.x, 5.0);

	//Pos.z = SafeMod(Pos.z, 5.0);

	return
		min(
			min(
				DistToKnot(Pos, knotCycle1, 0.0f, knotTubeRadius1, 0.10),
				DistToKnot(Pos, knotCycle2, 0.12f+knotTubeRadius1+knotTubeRadius2+knotTubeRadius3, knotTubeRadius2, 0.15)),
			DistToKnot(Pos, knotCycle3, 0.05f+knotTubeRadius1+knotTubeRadius2, knotTubeRadius3, 0.65));
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


float3 Shade(float3 inPos, float3 inNormal, float3 inEyeDir, float3 inEyePos, float inMatIndex, float2 inUV)
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
		
		float3 flatPos = inPos - inNormal*dot(inPos, inNormal);
		float2 uv = inUV.xy * 1.20;

		float3 texel = texture_even_lachen.SampleLevel(samplerTexture, uv.xy, 0).xyz;
		diffColor = texel * 0.90;

		ambient = diffColor*0.05*texel.b;
		specColor = (1.0).xxx;
		specAmount = 0.2 * texel.r;// + 0.05 * texel.b;
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

	float lightAttenuation = 1.0;// / (1.0 + lightDist*lightDist*0.001 + lightDist*0.00002);	

	float shadowFactor = max(SoftShadowIq( inPos + inNormal*0.2, lightDir, 0.005, lightDist * 0.2, 6 ), 0.2);

	return lightAmount * lightAttenuation * shadowFactor * (pow(2.0, fadeAmount)-1.0);
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
		result.color = float4( Shade(hitPos.xyz, normal, -dir.xyz, origin, hitMat, hitUV), depth );
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
