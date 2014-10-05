#pragma once

// Output extra info for PIX? 
#define PIMP_USE_PIX_INFO 0

// Format for our backbuffer. 
// @glow: my ION netbook only supports DXGI_FORMAT_B8G8R8A8_UNORM, instead of the
//       more common DXGI_FORMAT_R8G8B8A8_UNORM format.
#define PIMP_BACKBUFFER_FORMAT_LIN		DXGI_FORMAT_B8G8R8A8_UNORM      //< Linear colorspace backbuffer format (only used for finding the best matching format)
#define PIMP_BACKBUFFER_FORMAT_GAMMA	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB //< sRGB colorspace backbuffer format (which is actually used for the created swapchain)

// Antialising settings. 
// Used both in Core and by Player to set up the initial swap chain.
#define D3D_ANTIALIAS_NUM_SAMPLES 1
#define D3D_ANTIALIAS_QUALITY 0
