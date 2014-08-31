#pragma once

#include "Settings.h"

namespace Pimp
{

#if PIMP_USE_PIX_INFO

class D3DPIXEvent
{
public:
	D3DPIXEvent(const std::string& name)
	{
		D3DPERF_BeginEvent( 0x00000000, StrToWStr(name).c_str() );
	}

	~D3DPIXEvent()
	{
		D3DPERF_EndEvent();
	}
};

#define D3D_PIX_SCOPE( name ) D3DPIXEvent(name);
#define D3D_PIX_MARKER( name ) D3DPERF_SetMarker( 0x00000000, StrToWStr(name).c_str() );

#else

#define D3D_PIX_SCOPE( name ) 
#define D3D_PIX_MARKER( name ) 

#endif

} // namespace