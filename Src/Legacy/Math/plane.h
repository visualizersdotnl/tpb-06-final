
#pragma once

#include "vector3.h"

class Plane
{
public:

	Plane()
	{}

	Plane(float a, float b, float c, float d)
		: a(a),b(b),c(c),d(d)
	{}

	float DotCoord(const Vector3& v) const
	{
		return a*v.x + b*v.y + c*v.z + d;
	}


	Halfspace ClassifyPoint(const Vector3& v) const
	{
		float t = DotCoord(v);

		if (t < 0) 
			return HALFSPACE_NEGATIVE;
		if (t > 0)
			return HALFSPACE_POSITIVE;
		
		return HALFSPACE_ON_PLANE;
	}

	void Normalize()
	{
		float norm = sqrtf(a * a + b * b + c * c);

		if (norm != 0.0f)
		{
			a /= norm;
			b /= norm;
			c /= norm;
			d /= norm;
		}
		else
		{
			a = 0.0f;
			b = 0.0f;
			c = 0.0f;
			d = 0.0f;
		}
	}

	float a,b,c,d;
};
