
#pragma once

// http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
inline int IsPowerOfTwo(unsigned int x)
{
	return ((x != 0) && ((x & (~x + 1)) == x));
}

#if defined(_DEBUG) || defined(_DESIGN) // FIXME: _DESIGN is a Player build type.
	#define DEBUG_LOG(f,...) { char s[256]; sprintf_s(s, 256, f, __VA_ARGS__); OutputDebugString(s); OutputDebugString("\n"); }
#else
	#define DEBUG_LOG(f,...) 
#endif 
