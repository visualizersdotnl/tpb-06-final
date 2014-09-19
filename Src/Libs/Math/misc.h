#pragma once

#include "vector3.h"

enum Halfspace
{
	HALFSPACE_NEGATIVE = -1,
	HALFSPACE_ON_PLANE = 0,
	HALFSPACE_POSITIVE = 1
};


bool FloatsEqual(float a, float b, float epsilon);

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

float Clamp(float v, float a, float b);

inline int Clamp(int v, int a, int b) { return (v <= a) ? a : ((v >= b) ? b : v); }

Vector3 Clamp(const Vector3& v, const Vector3& minVec, const Vector3& maxVec);


#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795f
#endif

#ifndef M_PI_2
#define M_PI_2 6.283185307179586476925286766559f
#endif

#ifndef M_HALFPI
#define M_HALFPI 1.5707963267948966192313216916398f
#endif


#define DEG2RAD(x) ((x)*(M_PI/180.0f))
#define RAD2DEG(x) ((x)*(180.0f/M_PI))


// float random number generated from @LX/FRequency 
// (and others...)
// http://code4k.blogspot.com/2009/10/random-float-number-using-x86-asm-code.html
//
// provide a seed that's != 0
// returns a float in [-1,+1)
float sfrand( int *seed );

// same as sfrand, but returns a float in [0, 1)
float sfrand_normalized(int *seed );


// calculate two amounts that together form a crossfade:
// - when balance =  0 --> amountA = 1, amountB = 1
// - when balance = -1 --> amountA = 1, amountB = 0
// - when balance =  1 --> amountA = 0, amountB = 1
void CrossFade(float balance, float* amountA, float* amountB);


template <typename T>
T Lerp(T from, T to, float alpha)
{
	return from*(1.0f-alpha) + to*alpha;
}


template <typename T>
T Smerp(T from, T to, float alpha)
{
	// (x^2)*(3-2*x)
	return Lerp(from, to, alpha*alpha*(3.0f - 2.0f*alpha));
}
