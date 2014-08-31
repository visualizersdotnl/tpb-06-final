#include "StdAfx.h"
#include "quaterniontools.h"

// Inspired by Wine's implementation of D3DXQuaternionRotationMatrix:
// http://source.winehq.org/source/dlls/d3dx9_36/math.c#L1284
Quaternion CreateQuaternionFromRotationMatrix(const Matrix4& rotMatrix)
{
	Quaternion result;

	float trace = rotMatrix.m[0][0] + rotMatrix.m[1][1] + rotMatrix.m[2][2] + 1.0f;
	if (trace > 1.0f)
	{
		result.x = (rotMatrix.m[1][2] - rotMatrix.m[2][1]) / (2.0f * sqrtf(trace));
		result.y = (rotMatrix.m[2][0] - rotMatrix.m[0][2]) / (2.0f * sqrtf(trace));
		result.z = (rotMatrix.m[0][1] - rotMatrix.m[1][0]) / (2.0f * sqrtf(trace));
		result.w = sqrtf(trace) / 2.0f;
		return result;
	}

	int maxi = 0;
	float maxdiag = rotMatrix.m[0][0];

	for (int i=1; i<3; i++)
	{
		if (rotMatrix.m[i][i] > maxdiag)
		{
			maxi = i;
			maxdiag = rotMatrix.m[i][i];
		}
	}

	switch (maxi)
	{
	case 0:
		{
			float S = 2.0f * sqrtf(1.0f + rotMatrix.m[0][0] - rotMatrix.m[1][1] - rotMatrix.m[2][2]);
			result.x = 0.25f * S;
			result.y = (rotMatrix.m[0][1] + rotMatrix.m[1][0]) / S;
			result.z = (rotMatrix.m[0][2] + rotMatrix.m[2][0]) / S;
			result.w = (rotMatrix.m[1][2] - rotMatrix.m[2][1]) / S;
		}
		break;
	case 1:
		{
			float S = 2.0f * sqrtf(1.0f + rotMatrix.m[1][1] - rotMatrix.m[0][0] - rotMatrix.m[2][2]);
			result.x = (rotMatrix.m[0][1] + rotMatrix.m[1][0] ) / S;
			result.y = 0.25f * S;
			result.z = (rotMatrix.m[1][2] + rotMatrix.m[2][1] ) / S;
			result.w = (rotMatrix.m[2][0] - rotMatrix.m[0][2] ) / S;
		}
		break;
	case 2:
		{
			float S = 2.0f * sqrtf(1.0f + rotMatrix.m[2][2] - rotMatrix.m[0][0] - rotMatrix.m[1][1]);
			result.x = (rotMatrix.m[0][2] + rotMatrix.m[2][0] ) / S;
			result.y = (rotMatrix.m[1][2] + rotMatrix.m[2][1] ) / S;
			result.z = 0.25f * S;
			result.w = (rotMatrix.m[0][1] - rotMatrix.m[1][0] ) / S;
		}
		break;
	}

	return result;
}