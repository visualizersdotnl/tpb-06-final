
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

cbuffer paramsOnlyOnce
{
	float4x4 viewInvMatrix;
	float sceneRenderLOD = 1;	
	float2 quadScaleFactor;			// Scaling factor to render our full screen quad with a different aspect ratio (X=1, Y<=1)

	float fxTimeGlobal = 0;
};

VSOutput MainVS(VSInput input)
{ 
	VSOutput output;

	output.screenPos = float4(input.position.xy * quadScaleFactor, 0, 1);
	output.rayDir = float4(input.position.xy, 1.0, 1.0);
	
	return output;
}

SamplerState samplerTexture
{
	AddressU = WRAP;
	AddressV = WRAP;
	Filter = MIN_MAG_MIP_LINEAR;
};

// RIPPED FROM SHADERTOY

#if 1

// "The Inversion Machine" by Kali

static const float width=.22;
static const float scale=4.;
static const float detail=.002;

static float3 lightdir=-float3(.2,.5,1.);

static float2x2 rot;

float rand(float2 co){
	return frac(sin(dot(co.xy ,float2(12.9898,78.233))) * 43758.5453);
}

float de(float3 p) {
	float t=fxTimeGlobal;
	float dotp=dot(p,p);
	p.x+=sin(t*40.)*.007;
	p=p/dotp*scale;
	p=sin(p+float3(sin(1.+t)*2.,-t,-t*2.));
	float d=length(p.yz)-width;
	d=min(d,length(p.xz)-width);
	d=min(d,length(p.xy)-width);
	d=min(d,length(p*p*p)-width*.3);
	return d*dotp/scale;
}

float3 normal(float3 p) {
	float3 e = float3(0.0,detail,0.0);
	
	return normalize(float3(
			de(p+e.yxx)-de(p-e.yxx),
			de(p+e.xyx)-de(p-e.xyx),
			de(p+e.xxy)-de(p-e.xxy)
			)
		);	
}

float light(in float3 p, in float3 dir) {
	float3 ldir=normalize(lightdir);
	float3 n=normal(p);
	float sh=1.;
	float diff=max(0.,dot(ldir,-n))+.1*max(0.,dot(normalize(dir),-n));
	float3 r = reflect(ldir,n);
	float spec=max(0.,dot(dir,-r))*sh;
	return diff+pow(spec,20.)*.7;	
}

float raymarch(in float3 from, in float3 dir, in float2 screenPos) 
{
	float2 uv = screenPos.xy;
	uv.xy -= .5;
//	vec2 uv = gl_FragCoord.xy / iResolution.xy*2.-1.;
//	uv.y*=iResolution.y/iResolution.x;

	float st,d,col,totdist=st=0.;
	float3 p;
	float ra=rand(uv.xy*fxTimeGlobal)-.5;
	float ras=max(0.,sign(-.5+rand(float2(1.3456,.3573)*floor(30.+fxTimeGlobal*20.))));
	float rab=rand(float2(1.2439,2.3453)*floor(10.+fxTimeGlobal*40.))*ras;
	float rac=rand(float2(1.1347,1.0331)*floor(40.+fxTimeGlobal));
	float ral=rand(1.+floor(uv.yy*300.)*fxTimeGlobal)-.5;
	for (int i=0; i<60; i++) {
		p=from+totdist*dir;
		d=de(p);
		if (d<detail || totdist>2.) break;
		totdist+=d; 
		st+=max(0.,.04-d);
	}
	float2 li=mul(rot, uv);
	float backg=.45*pow(1.5-min(1.,length(li+float2(0.,-.6))),1.5);
	if (d<detail) {
		col=light(p-detail*dir, dir); 
	} else { 
		col=backg;
	}
	col+=smoothstep(0.,1.,st)*.8*(.1+rab);
	col+=pow(max(0.,1.-length(p)),8.)*(.5+10.*rab);
	col+=pow(max(0.,1.-length(p)),30.)*50.;
	col = lerp(col, backg, 1.0-exp(-.25*pow(totdist,3.)));
	if (rac>.7) col=col*.7+(.3+ra+ral*.5)*fmod(uv.y+fxTimeGlobal*2.,.25);
	col = lerp(col, .5+ra+ral*.5, max(0.,3.-fxTimeGlobal)/3.);
	return col+ra*.03+(ral*.1+ra*.1)*rab;
}

float4 ripped_Main(float2 screenPos)
{
	float t=fxTimeGlobal*.2;
	float2 res = { 1920, 1080 };
	float2 uv = screenPos.xy / res*2-1;//gl_FragCoord.xy / iResolution.xy*2.-1.;
//	uv.y*=iResolution.y/iResolution.x;
	float3 from=float3(0.,0.1,-1.2);
	float3 dir=normalize(float3(uv,1.));
	rot=float2x2(cos(t),sin(t),-sin(t),cos(t));
//	dir.xy=dir.xy*rot;
	dir.xy = mul(rot, dir.xy);
	float col=raymarch(from,dir, screenPos); 
	col=pow(col,1.25)*clamp(60.-fxTimeGlobal,0.,1.);
	return float4(col.xxxx);
}

#endif

// ^^ RIPPED FROM SHADERTOY

PSOutput MainPS(VSOutput input)
{
	PSOutput result;

	float4 screenPos = input.screenPos;
	result.color = ripped_Main(float2(screenPos.x, screenPos.y));

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
