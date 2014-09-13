
// This header contains some stuff used or exposed by the new stub.

// It generally only needs to be included in places where: 
// - SetLastError() is called.
// - The window handle or World pointer are needed.

#pragma once

// COM release macro
#define SAFE_RELEASE(pX) if (NULL != (pX)) (pX)->Release()

// serialize constant value to std::string
// @plek: This is sort of ugly but I didn't port all to string streams in the original project.
template<typename T> inline std::string ToString(const T &X)
{
	std::stringstream strStream;
//	VERIFY((strStream << X));
	return strStream.str();
}

// declared in Stub.cpp
void SetLastError(const std::string &message);

// declared in Stub.cpp
extern HWND g_hWnd;
extern Pimp::World *gWorld;
