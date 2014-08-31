#pragma once


void * GetWindowLongUserPtrWrapped(HWND hwnd);
void SetWindowLongUserPtrWrapped(HWND hwnd, void * userPtr);


// http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
inline int IsPowerOfTwo(unsigned int x)
{
	return ((x != 0) && ((x & (~x + 1)) == x));
}


#ifdef _DEBUG

#define DEBUG_LOG(f,...) do { char s[256]; sprintf_s(s,256,f, __VA_ARGS__); OutputDebugString(s); OutputDebugString("\n"); } while(false);

#else

#define DEBUG_LOG(f,...) 

#endif 