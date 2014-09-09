#pragma once

#include "quaternion.h"
#include "matrix4.h"

extern Quaternion CreateQuaternionFromRotationMatrix(const Matrix4& rotMatrix);
extern Quaternion CreateQuaternionFromYawPitchRoll(float yaw, float pitch, float roll);