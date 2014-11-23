#pragma once


class Vector4
{
public:

	Vector4() {}
	Vector4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w)
	{}


	Vector4 Cross(const Vector4& other1, const Vector4& other2) const
	{
		Vector4 out;
		out.x = this->y * (other1.z * other2.w - other2.z * other1.w) - this->z * (other1.y * other2.w - other2.y * other1.w) + this->w * (other1.y * other2.z - other1.z *other2.y);
		out.y = -(this->x * (other1.z * other2.w - other2.z * other1.w) - this->z * (other1.x * other2.w - other2.x * other1.w) + this->w * (other1.x * other2.z - other2.x * other1.z));
		out.z = this->x * (other1.y * other2.w - other2.y * other1.w) - this->y * (other1.x *other2.w - other2.x * other1.w) + this->w * (other1.x * other2.y - other2.x * other1.y);
		out.w = -(this->x * (other1.y * other2.z - other2.y * other1.z) - this->y * (other1.x * other2.z - other2.x *other1.z) + this->z * (other1.x * other2.y - other2.x * other1.y));
		return out;
	}

	bool operator==(const Vector4& b) const
	{
		return x==b.x && y==b.y && z==b.z && w==b.w;
	}

	bool operator!=(const Vector4& b) const
	{
		return x!=b.x || y!=b.y || z!=b.z || w!=b.w;
	}

	float x,y,z,w;
};

