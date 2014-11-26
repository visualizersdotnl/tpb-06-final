#include "misc.h"

bool FloatsEqual(float a, float b, float epsilon)
{
	float delta = a-b;
	
	return ((delta <= epsilon) && (delta >= -epsilon));
}




float Clamp(float v, float a, float b)
{
	if (v <= a)
		return a;
	else if (v >= b)
		return b;
	else 
		return v;
}


Vector3 Clamp(const Vector3& v, const Vector3& minVec, const Vector3& maxVec)
{
	return Vector3(
		Clamp(v.x, minVec.x, maxVec.x),
		Clamp(v.y, minVec.y, maxVec.y),
		Clamp(v.z, minVec.z, maxVec.z));
}



float sfrand( int *seed )
{
	*seed *= 16807;
	return ((float)*seed) / (float)0x80000000;
}

// returns rand value from [0,1)
float sfrand_normalized(int *seed )
{
	return sfrand(seed)*0.5f+0.5f;
}


void CrossFade(float balance, float* amountA, float* amountB)
{
	*amountA = (balance >= 0) ? (1-balance) : 1;
	*amountB = (balance <= 0) ? (balance+1) : 1;		
}
