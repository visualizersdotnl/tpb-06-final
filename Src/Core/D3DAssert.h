
#pragma once

// FIXME: location is deprecated.
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\DxErr.h"

class CoreD3DException : public std::exception
{
public:
	CoreD3DException(const std::string &error) : 
		exception(), 
		error(error)
		{
		}

		const char *what() const { return error.c_str(); }

private:
	std::string error;
};

// Compose message using DxErr
inline const std::string GetDxErrorDesc(HRESULT hRes, const char *fileName, int lineNo)
{
	std::stringstream message;
	message << fileName << " (" << lineNo << ")\n";
	message << "Direct3D call failed (" << DXGetErrorString(hRes) << "): " << DXGetErrorDescription(hRes) << "\n";
	return message.str();
}

// Custom message
inline const std::string GetDxErrorDesc(const std::string &error, const char *fileName, int lineNo)
{
	std::stringstream message;
	message << fileName << " (" << lineNo << ")\n";
	message << "Direct3D error: " << error << "\n";
	return message.str();
}

#ifdef _DEBUG
	#define D3D_ASSERT(hRes) ASSERT_MSG(S_OK == (hRes), (S_OK != (hRes)) ? GetDxErrorDesc(hRes, __FILE__, __LINE__).c_str() : "")
	#define D3D_VERIFY(hRes) D3D_ASSERT(hRes)
	#define D3D_ASSERT_MSG(hRes, message) ASSERT_MSG(S_OK == (hRes), GetDxErrorDesc(message, __FILE__, __LINE__).c_str())
#else
	#define D3D_ASSERT(hRes) if (S_OK != (hRes)) throw CoreD3DException(GetDxErrorDesc(hRes, __FILE__, __LINE__))
	#define D3D_VERIFY(hRes) (hRes) // This is a development check, so don't throw.
	#define D3D_ASSERT_MSG(hRes, message) if (S_OK != (hRes)) throw CoreD3DException(GetDxErrorDesc(message, __FILE__, __LINE__))
#endif
