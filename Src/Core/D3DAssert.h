
#pragma once

// FIXME: this is deprecated.
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\DxErr.h"

#include <exception>

class CoreD3DException : public std::exception
{
public:
	CoreD3DException(const std::string &errMsg) : 
		exception(), 
		m_errMsg(errMsg)
		{
		}

		const char *what() const { return m_errMsg.c_str(); }

private:
	std::string m_errMsg;
};

inline const std::string GetDxErrorDesc(HRESULT hRes)
{
	std::string message;
	message  = "Direct3D call failed (";
	message += DXGetErrorString(hRes);
	message += "): ";
	message += DXGetErrorDescription(hRes);
	message += "\n";
	return message;
}

#ifdef _DEBUG
//	#define D3D_ASSERT(hRes) if (S_OK != (hRes)) throw CoreD3DException(GetDxErrorDesc(hRes))
	#define D3D_ASSERT(hRes) ASSERT_MSG(S_OK == (hRes), (S_OK != (hRes)) ? GetDxErrorDesc((hRes)).c_str() : "")
	#define D3D_VERIFY(hRes) D3D_ASSERT(hRes)
	#define D3D_ASSERT_MSG(hRes, message) ASSERT_MSG(S_OK == (hRes), message)
#else
	#define D3D_ASSERT(hRes) if (S_OK != (hRes)) throw CoreD3DException(GetDxErrorDesc(hRes))
	#define D3D_VERIFY(hRes) (hRes) // This is a development check, so don't throw.
	#define D3D_ASSERT_MSG(hRes, message) if (S_OK != (hRes)) throw CoreD3DException("Direct3D error: " + std::string(message) + "\n")
#endif
