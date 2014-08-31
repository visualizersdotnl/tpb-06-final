#pragma once


#ifndef _DEBUG
#define PIMP_USE_ERROR_REPORTING 0
#else
#define PIMP_USE_ERROR_REPORTING 1
#endif

#ifndef _DEBUG
#define PIMP_SUPPORT_DUMPANIMS 0
#else
#define PIMP_SUPPORT_DUMPANIMS 1
#endif

// Output extra info for PIX? 
#ifndef _DEBUG
#define PIMP_USE_PIX_INFO 0
#else
#define PIMP_USE_PIX_INFO 0 // Keep disabled for now, only worked in dx9?
#endif

// Format for our backbuffer. 
// Note: my ION netbook only supports DXGI_FORMAT_B8G8R8A8_UNORM, instead of the
// more common DXGI_FORMAT_R8G8B8A8_UNORM format.
#define PIMP_BACKBUFFER_FORMAT DXGI_FORMAT_B8G8R8A8_UNORM
