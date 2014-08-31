#pragma once

#include "stdafx.h"
#include "vector3.h"

/*
A simple 4x4 float matrix class. Note that's actually a 4x3 matrix (4 rows, 3 columns),
since the last columns is assumed to be (0,0,0,1)
*/
class Matrix4
{
public:

	void SetIdentity();

	Vector3 TransformNormal(const Vector3& v) const
	{
		return Vector3(
				m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
				m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
				m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z
			);
	}

	Vector3 TransformCoord(const Vector3& v) const
	{
		return Vector3(
			m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0],
			m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1],
			m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2]
			);
	}

	Matrix4 operator*(const Matrix4& other) const
	{
		Matrix4 newMat;

		for (int i=0; i<4; i++)
			for (int j=0; j<4; j++)
				newMat.m[i][j] = 
					m[i][0] * other.m[0][j] + 
					m[i][1] * other.m[1][j] + 
					m[i][2] * other.m[2][j] + 
					m[i][3] * other.m[3][j];

		return newMat;
	}

	Matrix4& operator*=(const Matrix4& other)
	{
		Matrix4 newMat( *this * other );
		*this = newMat;
		return *this;
	}

	Matrix4 Transposed() const
	{
		Matrix4 newMat;

		for (int i=0; i<4; i++)
			for (int j=0; j<4; j++)
				newMat.m[i][j] = m[j][i];

		return newMat;
	}

	void Transpose()
	{
		*this = Transposed();
	}

	void InvertOrthogonal()
	{
		Transpose();
	}

	Matrix4 InvertedOrthogonal() const
	{
		Matrix4 m(*this);
		m.InvertOrthogonal();
		return m;
	}

	void Invert()
	{
		*this = Inversed();
	}

	Matrix4 Inversed() const;

	float Determinant() const;

	Vector3 GetTranslation() const 
	{
		return Vector3(m[3][0],m[3][1],m[3][2]);
	}

	void SetTranslation(const Vector3& v)
	{
		m[3][0] = v.x;
		m[3][1] = v.y;
		m[3][2] = v.z;
	}

	static void Init();

	static const Matrix4& GetIdentity()
	{
		return IDENTITY;
	}

	static const Matrix4* GetIdentityPtr()
	{
		return &IDENTITY;
	}

	union 
	{
		struct 
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};

		float m[4][4];
	};

private:
	static Matrix4 IDENTITY;
};
