#include "D3DException.h"

namespace Pimp
{

	//
	// in case of extreme size problems, we could even
	// disable the errorchecking completely, by 
	// saying: #define D3D_CHECK(x,y) (x)
	//

#if PIMP_USE_ERROR_REPORTING

	D3DException::D3DException(HRESULT hr, const std::string& location)
		: Exception("")
	{
		msg = "D3D Error at " + location + ":\n";

		char s[256];
		sprintf_s(s,256,"Code=%X\n", hr);
		
		msg += std::string(s);
	}


#endif



}