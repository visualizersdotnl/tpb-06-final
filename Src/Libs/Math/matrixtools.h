#pragma once

#include "matrix4.h"
#include "vector3.h"
#include "quaternion.h"

// RH
extern Matrix4 CreateMatrixPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

// RH
extern Matrix4 CreateMatrixLookAt(const Vector3& position, const Vector3& forward, const Vector3& up);

extern Matrix4 CreateMatrixTranslation(const Vector3& translation);
extern Matrix4 CreateMatrixRotationX(float angle);
extern Matrix4 CreateMatrixRotationY(float angle);
extern Matrix4 CreateMatrixRotationZ(float angle);
extern Matrix4 CreateMatrixRotationQuaternion(const Quaternion& q);
extern Matrix4 CreateMatrixScaling(float scale);
extern Matrix4 CreateMatrixScaling(const Vector3& scale);



