#include "StdAfx.h"
#include "quaternion.h"

void strfloat(char*, float f);

// http://bediyap.com/programming/convert-quaternion-to-euler-rotations/

Vector3 Quaternion::GetEulerAnglesXYZ() const
{
	Vector3 result;

	float r11 = 2.0f*(x*y + w*z);
	float r12 = w*w + x*x - y*y - z*z;
	float r21 = -2.0f*(x*z - w*y);
	float r31 = 2.0f*(y*z + w*x);
	float r32 = w*w - x*x - y*y + z*z;

	result.x = atan2( r31, r32 );
	result.y = asin( r21 );
	result.z = atan2( r11, r12 );

	return result;
}
