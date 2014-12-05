
// Default (post-processing) & ray marching vertex shader for a ScreenQuad.

struct VSInput
{
	float3 position : POSITION;
};

struct VSOutput
{
	float4 screenPos : SV_Position;
	float2 uv : TEXCOORD0;
};

// For ray marching.
struct VSOutput_RM 
{
	float4 screenPos : SV_Position;

	// World space view direction
	float4 rayDir : TEXCOORD0; 
};

// For ray marching.
cbuffer CoreScreenQuadConstants
{
	// FIXME: feed from actual ratio (available through Core::D3D).
	float renderAspect = 16.f/9.f;

#if !defined(MATERIAL_CONSTANTS_INC)
	float sceneRenderLOD = 1.f;
#endif
};

VSOutput ScreenQuadVS(VSInput input)
{ 
	VSOutput output;

	output.screenPos = float4(input.position.xy, 0.f, 1.f);
	output.uv = input.position.xy * float2(0.5f, -0.5f) + 0.5f;

	return output;
}

// For ray marching.
VSOutput_RM ScreenQuadVS_RM(VSInput input)
{ 
	VSOutput_RM output;

	output.screenPos = float4(input.position.xy * float2(1.f, renderAspect) * sceneRenderLOD, 0.f, 1.f);
	output.rayDir = float4(input.position.xy, 0.f, 0.f);
	
	return output;
}
