#pragma once

#include <math.h>


class Vector3
{
public:
	Vector3()
	{}

	Vector3(float v)
		: x(v),y(v),z(v)
	{}

	Vector3(float x, float y, float z)
		: x(x),y(y),z(z)
	{}

	Vector3(const Vector3& v)
		: x(v.x),y(v.y),z(v.z)
	{
	}

	float Length() const
	{
		return sqrtf(x*x+y*y+z*z);
	}

	float LengthSq() const
	{
		return x*x+y*y+z*z;
	}

	float Dot(const Vector3& v) const
	{
		return x*v.x+y*v.y+z*v.z;
	}

	Vector3 Cross(const Vector3& v) const
	{
		return Vector3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}

	void Normalize()
	{
		float length = sqrtf(x*x+y*y+z*z);

		x /= length;
		y /= length;
		z /= length;
	}

	Vector3 Normalized() const
	{
		Vector3 v(*this);
		v.Normalize();
		return v;
	}

	Vector3 operator+(const Vector3& v) const
	{
		return Vector3(x+v.x,y+v.y,z+v.z);
	}

	Vector3& operator+=(const Vector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3 operator-(const Vector3& v) const
	{
		return Vector3(x-v.x,y-v.y,z-v.z);
	}

	Vector3& operator-=(const Vector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector3 operator*(float s) const
	{
		return Vector3(x*s,y*s,z*s);
	}

	Vector3& operator*=(float s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	Vector3 operator*(const Vector3& v) const
	{
		return Vector3(x*v.x,y*v.y,z*v.z);
	}

	Vector3& operator*=(const Vector3& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	Vector3 operator/(float s) const
	{
		return Vector3(x/s,y/s,z/s);
	}

	Vector3& operator/=(float s)
	{
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}

	static const Vector3& GetZero()
	{
		return zero;
	}

	static const Vector3& GetOne()
	{
		return one;
	}

	float x,y,z;

private:
	static Vector3 zero;
	static Vector3 one;
};