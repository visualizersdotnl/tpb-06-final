
// Constants required by .FX shaders contained in a Material instance.

#ifndef _MATERIAL_CONSTANTS_
#define _MATERIAL_CONSTANTS_

cbuffer CoreMaterialConstants
{
	float4x4 viewInvMatrix; 
	float sceneRenderLOD = 1.f;
};

// Only to be used by user post processing FX.
Texture2D sceneBuffer;

#endif // _MATERIAL_CONSTANTS_

