
#pragma once

#include <math.h>

class Vector2
{
public:
	Vector2() {}

	Vector2(float x, float y)
		: x(x), y(y)
	{}

	Vector2 operator*(float scale) const
	{
		return Vector2(x*scale, y*scale);
	}

	Vector2 operator+(const Vector2& other) const
	{
		return Vector2(x+other.x, y+other.y);
	}

	Vector2 operator-(const Vector2& other) const
	{
		return Vector2(x-other.x, y-other.y);
	}


	Vector2 operator/(float s) const
	{
		return Vector2(x/s,y/s);
	}

	Vector2& operator/=(float s)
	{
		x /= s;
		y /= s;
		return *this;
	}

	float Length() const
	{
		return sqrtf(x*x+y*y);
	}

	float LengthSq() const
	{
		return x*x+y*y;
	}

	float Dot(const Vector2& other) const
	{
		return x*other.x + y*other.y;
	}

	float x,y;
};
