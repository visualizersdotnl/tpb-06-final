
// Constants required by .FX shaders contained in a Material instance.

#ifndef _MATCONSTS_
#define _MATCONSTS_

cbuffer paramsMaterial
{
	float4x4 viewInvMatrix; 
	float sceneRenderLOD = 1.f;
};

// Only to be used by user post processing FX.
Texture2D sceneBuffer;

#endif
