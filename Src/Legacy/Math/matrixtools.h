
#pragma once

#include "matrix4.h"
#include "vector3.h"
#include "quaternion.h"

// RH
Matrix4 CreateMatrixPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

// RH
Matrix4 CreateMatrixLookAt(const Vector3& position, const Vector3& forward, const Vector3& up);

Matrix4 CreateMatrixTranslation(const Vector3& translation);
Matrix4 CreateMatrixRotationX(float angle);
Matrix4 CreateMatrixRotationY(float angle);
Matrix4 CreateMatrixRotationZ(float angle);
Matrix4 CreateMatrixRotationQuaternion(const Quaternion& q);
Matrix4 CreateMatrixScaling(float scale);
Matrix4 CreateMatrixScaling(const Vector3& scale);
