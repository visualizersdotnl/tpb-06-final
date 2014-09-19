#pragma once




class Quaternion
{
public:

	Quaternion()
	{}

	Quaternion(float x, float y, float z, float w)
		: x(x),y(y),z(z),w(w)
	{}

	Quaternion operator*(const Quaternion& other) const
	{
		Quaternion result;

		result.x = other.w * this->x + other.x * this->w + other.y * this->z - other.z * this->y;
		result.y = other.w * this->y - other.x * this->z + other.y * this->w + other.z * this->x;
		result.z = other.w * this->z + other.x * this->y - other.y * this->x + other.z * this->w;
		result.w = other.w * this->w - other.x * this->x - other.y * this->y - other.z * this->z;

		return result;		
	}

	Quaternion& operator*=(const Quaternion& other)
	{
		Quaternion q = *this * other;
		*this = q;
		return *this;
	}

	Vector3 GetEulerAnglesXYZ() const;

	float x,y,z,w;
};