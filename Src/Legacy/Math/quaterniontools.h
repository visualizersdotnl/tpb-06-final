
#pragma once

#include "quaternion.h"
#include "matrix4.h"

Quaternion CreateQuaternionFromRotationMatrix(const Matrix4& rotMatrix);
Quaternion CreateQuaternionFromYawPitchRoll(float yaw, float pitch, float roll);
