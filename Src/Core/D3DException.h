#pragma once

#include "Settings.h"
#include <shared/shared.h>

namespace Pimp
{
#if PIMP_USE_ERROR_REPORTING

	class D3DException : public Exception
	{
	public:
		D3DException(HRESULT hr, const std::string& location);
	};

#define D3D_VERIFY( result, location ) \
	if (result != S_OK) \
		throw Pimp::D3DException(result, location);


	//#include <DxErr.h>
	//#define DX_ASSERT(hr) ASSERT_MSG(hr == S_OK, "Code: "+std::string(DXGetErrorString(hr))+"\nDescription: "+std::string(DXGetErrorDescription(hr)))

#define D3D_ASSERT( result ) if (result != S_OK) { char s[512]; sprintf_s(s,512, "D3D ERROR! HRESULT=%X", result); ASSERT_MSG(result == S_OK, s); }
#define D3D_ASSERT_MSG( result, msg ) if (result != S_OK) { char s[512]; sprintf_s(s,512, "D3D ERROR! HRESULT=%X\nMessage: %s", result, msg); ASSERT_MSG(result == S_OK, s); }

#else // PIMP_USE_ERROR_REPORTING

#define D3D_VERIFY( result, location ) result
#define D3D_ASSERT( result )
#define D3D_ASSERT_MSG( result, msg )

#endif
}