
#if !defined(MATERIAL_CONSTANTS_INC)
#define MATERIAL_CONSTANTS_INC
 
// Constants required by .FX shaders contained in a Material instance.
cbuffer CoreMaterialConstants
{
	float4x4 viewInvMatrix; 
	float sceneRenderLOD = 1.f;
};

// Only to be used by user post processing FX.
Texture2D sceneBuffer;

#endif // MATERIAL_CONSTANTS_INC
