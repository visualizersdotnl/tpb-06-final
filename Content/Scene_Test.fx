
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
	float fisheyeFOV = 1.0; //1.0;
	
	float fadeAmount = 1; // 0 = black, 1 = visible, 10 = white-ish

	float4 testLightPos = float4(20.0, 20.0, 20.0, 1.0);
	float4 testBallPos = float4(0.0, -20.0, 0.0, 1.0);
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


Texture2D texture_1917884299; // Perlin
Texture2D texture_4734255; // Sand


SamplerState samplerDisplaceTexture
{
	AddressU = WRAP;
	AddressV = WRAP;
	Filter = MIN_MAG_MIP_LINEAR;
};





float DistSmooth(float a, float b, out float matIndex)
{
	float k = 0.04;
	
	float d = b-a;
	
	// if a is much smaller than b, take a
	if (d >= k)
	{
		matIndex = 0;
		return a;
	}
	// if b is much smaller than a, take b
	else if (d <= -k)
	{
		matIndex = 1;
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

		matIndex = alpha;
		
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





float DistanceEstimator(float3 Pos, out float HitMat) 
{
	float d0 = -DistToBoxSigned(Pos, (25.0).xxx);
	float d1 = DistToSphere(Pos - testBallPos, 5.0);

	if (d0 < d1)
	{
		HitMat = 0;
		return d0;		
	}
	else
	{
		HitMat = 1;
		return d1;
	}
}





bool Trace(float4 From, float4 Dir, out float4 HitPos, out float HitMaterial)
{
	float dist = 0.0;
	float totalDist = 0.0;
	float stepEpsilon = 0.0;

	float4 pos = From;
	int step = 0;
	HitMaterial = 0;

	[fastopt] while (step < MaxRaySteps)
	{
		dist = DistanceEstimator(pos.xyz, HitMaterial);
		
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
	float3 eps = float3(0.0, 10e-5, 0.0);
	float dummy = 0;
	
	float3 n = float3(
		DistanceEstimator(Pos+eps.yxx, dummy) - DistanceEstimator(Pos-eps.yxx, dummy),
		DistanceEstimator(Pos+eps.xyx, dummy) - DistanceEstimator(Pos-eps.xyx, dummy),
		DistanceEstimator(Pos+eps.xxy, dummy) - DistanceEstimator(Pos-eps.xxy, dummy));
	
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

  for (int step = 0; step < MaxSoftShadowIqSteps; step++)
  {
      float d = DistanceEstimator(inPos + inLightDir*t, dummy);
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


float3 Shade(float3 inPos, float3 inNormal, float3 inEyeDir, float3 inEyePos, float inMatIndex)
{
	float3 lightOffset = (testLightPos.xyz - inPos);
	
	float lightDist = length(lightOffset);
	float3 lightDir = normalize(lightOffset);
	
	float diffuseTerm = dot(inNormal, lightDir);


	float3 diffColor;
	float3 ambient;
	float specAmount;
	float3 maxLight;

	if (inMatIndex < 1)
	{		
		// Walls and floor
		
		float gridFreq = 1.0;
		float gridThickness = 0.020;
		float gridPattern = max(max(sin(inPos.x * gridFreq),sin(inPos.y * gridFreq)),sin(inPos.z * gridFreq));
		float grid = max(0.0, gridPattern-(1.0-gridThickness))/gridThickness;	

		diffColor = (0.3 + grid*0.5).xxx;
		ambient = diffColor*0.05;//(0.1).xxx;	
		specAmount = 0.6;
	}
	else
	{
		// Ball

		diffColor = ColOrange * 0.6;
		ambient = ColOrange*0.2;
		specAmount = 0.2;
	}


	float3 diffuse = max(diffuseTerm, 0.0) * diffColor;
	
	float3 reflect = normalize((2.0f * diffuseTerm * inNormal) - lightDir);
	float specular = pow( max(0, dot(reflect, inEyeDir) ), 1 ) * specAmount;	
	
	float3 lightAmount = min(
		diffuse + specular*diffColor + ambient,
		(1.0).xxx
		);

	float lightAttenuation = 1.0;// / (1.0 + lightDist*lightDist*0.001 + lightDist*0.00002);	

	float shadowFactor = max(SoftShadowIq( inPos + inNormal*1.1, lightDir, 0.005, lightDist * 0.2, 8 ), 0.7); 	

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

	if (Trace(origin, dir, hitPos, hitMat))
	{
		float3 normal = Normal(hitPos.xyz);

#if SHOW_NORMALS		
		result.color = float4(normal.xyz*0.5 + (0.5).xxx,1);		
#else
		result.color = float4( Shade(hitPos.xyz, normal, -dir.xyz, origin, hitMat), 1 );
#endif			
	}
	else
	{
		// Outside anything.
		result.color = float4(0,0,0,1);
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
