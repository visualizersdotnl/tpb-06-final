#include "stdafx.h"
#include "misc.h"

void * GetWindowLongUserPtrWrapped(HWND hwnd)
{
#pragma warning(disable : 4312)
	void * ptr = (void*)GetWindowLongPtr(hwnd, DWLP_USER);
#pragma warning(default : 4312)

	return ptr;
}

void SetWindowLongUserPtrWrapped(HWND hwnd, void * userPtr)
{
#pragma warning(disable : 4311)
	SetWindowLongPtr(hwnd, DWLP_USER, (LONG)userPtr);
#pragma warning(default : 4311)
}