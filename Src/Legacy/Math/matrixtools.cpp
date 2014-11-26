#include "matrixtools.h"


// Disclaimer:
// Several of these functions are inspired on Wine's implementation of various of its D3DXMatrix functions
// http://source.winehq.org/source/dlls/d3dx9_36/math.c


Matrix4 CreateMatrixPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	Matrix4 m = Matrix4::GetIdentity();

	m.m[0][0] = 1.0f / (aspect * tanf(fovY/2.0f));
	m.m[1][1] = 1.0f / tanf(fovY/2.0f);
	m.m[2][2] = farZ / (nearZ - farZ);
	m.m[2][3] = -1.0f;
	m.m[3][2] = (farZ * nearZ) / (nearZ - farZ);
	m.m[3][3] = 0.0f;

	return m;
}


Matrix4 CreateMatrixLookAt(const Vector3& position, const Vector3& forward, const Vector3& up)
{
	Vector3 right = up.Cross(forward);
	Vector3 correctedUp = forward.Cross(right);
	right.Normalize();
	correctedUp.Normalize();

	Matrix4 m;

	m.m[0][0] = -right.x;
	m.m[1][0] = -right.y;
	m.m[2][0] = -right.z;
	m.m[3][0] = right.Dot(position);
	m.m[0][1] = correctedUp.x;
	m.m[1][1] = correctedUp.y;
	m.m[2][1] = correctedUp.z;
	m.m[3][1] = -correctedUp.Dot(position);
	m.m[0][2] = -forward.x;
	m.m[1][2] = -forward.y;
	m.m[2][2] = -forward.z;
	m.m[3][2] = forward.Dot(position);
	m.m[0][3] = 0.0f;
	m.m[1][3] = 0.0f;
	m.m[2][3] = 0.0f;
	m.m[3][3] = 1.0f;

	return m;
}


Matrix4 CreateMatrixScaling(float scale)
{
	Matrix4 m = Matrix4::GetIdentity();

	m.m[0][0] = scale;
	m.m[1][1] = scale;
	m.m[2][2] = scale;

	return m;
}


Matrix4 CreateMatrixScaling(const Vector3& scale)
{
	Matrix4 m = Matrix4::GetIdentity();

	m.m[0][0] = scale.x;
	m.m[1][1] = scale.y;
	m.m[2][2] = scale.z;

	return m;
}


Matrix4 CreateMatrixRotationX(float angle)
{
	Matrix4 m = Matrix4::GetIdentity();

	m.m[1][1] = cosf(angle);
	m.m[2][2] = cosf(angle);
	m.m[1][2] = sinf(angle);
	m.m[2][1] = -sinf(angle);

	return m;
}


Matrix4 CreateMatrixRotationY(float angle)
{
	Matrix4 m = Matrix4::GetIdentity();

	m.m[0][0] = cosf(angle);
	m.m[2][2] = cosf(angle);
	m.m[0][2] = -sinf(angle);
	m.m[2][0] = sinf(angle);

	return m;
}


Matrix4 CreateMatrixRotationZ(float angle)
{
	Matrix4 m = Matrix4::GetIdentity();

	m.m[0][0] = cosf(angle);
	m.m[1][1] = cosf(angle);
	m.m[0][1] = sinf(angle);
	m.m[1][0] = -sinf(angle);

	return m;
}


Matrix4 CreateMatrixRotationQuaternion(const Quaternion& q)
{
	Matrix4 m = Matrix4::GetIdentity();

	m.m[0][0] = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	m.m[0][1] = 2.0f * (q.x *q.y + q.z * q.w);
	m.m[0][2] = 2.0f * (q.x * q.z - q.y * q.w);
	m.m[1][0] = 2.0f * (q.x * q.y - q.z * q.w);
	m.m[1][1] = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
	m.m[1][2] = 2.0f * (q.y *q.z + q.x *q.w);
	m.m[2][0] = 2.0f * (q.x * q.z + q.y * q.w);
	m.m[2][1] = 2.0f * (q.y *q.z - q.x *q.w);
	m.m[2][2] = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);

	return m;
}


Matrix4 CreateMatrixTranslation(const Vector3& translation)
{
	Matrix4 m = Matrix4::GetIdentity();

	m.m[3][0] = translation.x;
	m.m[3][1] = translation.y;
	m.m[3][2] = translation.z;

	return m;
}