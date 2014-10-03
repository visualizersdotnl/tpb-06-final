#include "stdafx.h"
#include "matrix4.h"
#include "vector4.h"
#include "misc.h"


Matrix4 Matrix4::IDENTITY;


void Matrix4::Init()
{
	IDENTITY.SetIdentity();
}


void Matrix4::SetIdentity()
{
	_11 = 1.0f;
	_12 = 0.0f;
	_13 = 0.0f;
	_14 = 0.0f;
	_21 = 0.0f;
	_22 = 1.0f;
	_23 = 0.0f;
	_24 = 0.0f;
	_31 = 0.0f;
	_32 = 0.0f;
	_33 = 1.0f;
	_34 = 0.0f;
	_41 = 0.0f;
	_42 = 0.0f;
	_43 = 0.0f;
	_44 = 1.0f;
}


// Inspired by Wine's implementation of D3DXMatrixInverse
// http://source.winehq.org/source/dlls/d3dx9_36/math.c#L227
Matrix4 Matrix4::Inversed() const
{
	Matrix4 result;

	Vector4 vec[3];

	float det = Determinant();

	ASSERT( !FloatsEqual(det, 0.0f, 0.0000001f) );

	static const float factor[4] = {
		1, -1, 1, -1
	};
	
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			if (j != i)
			{
				int a = j;

				if (j > i) 
					a -= 1;

				vec[a].x = m[j][0];
				vec[a].y = m[j][1];
				vec[a].z = m[j][2];
				vec[a].w = m[j][3];
			}
		}

		Vector4 v = vec[0].Cross(vec[1], vec[2]);

		result.m[0][i] = factor[i] * v.x / det;
		result.m[1][i] = factor[i] * v.y / det;
		result.m[2][i] = factor[i] * v.z / det;
		result.m[3][i] = factor[i] * v.w / det;
	}
	
	return result;
}

// Inspired by Wine's implementation of D3DXMatrixDeterminant
// http://source.winehq.org/source/dlls/d3dx9_36/math.c#L214
float Matrix4::Determinant() const
{
	Vector4 v1, v2, v3;

	v1.x = m[0][0]; v1.y = m[1][0]; v1.z = m[2][0]; v1.w = m[3][0];
	v2.x = m[0][1]; v2.y = m[1][1]; v2.z = m[2][1]; v2.w = m[3][1];
	v3.x = m[0][2]; v3.y = m[1][2]; v3.z = m[2][2]; v3.w = m[3][2];

	Vector4 minor = v1.Cross(v2, v3);
	return - (m[0][3] * minor.x + m[1][3] * minor.y + m[2][3] * minor.z + m[3][3] * minor.w);
}


//// Polar decomposition, based on:
//// http://callumhay.blogspot.nl/2010/10/decomposing-affine-transforms.html
//void Matrix4::PolarDecompose(Vector3* outTranslation, Matrix4* outRotation, Vector3* outScale)
//{
//	*outTranslation = Vector3(m[3][0], m[3][1], m[3][2]);
//
//	Matrix4 m = *this;
//	m.m[3][0] = 0;
//	m.m[3][1] = 0;
//	m.m[3][2] = 0;
//
//	// Extract the rotation component - this is done using polar decompostion, where
//	// we successively average the matrix with its inverse transpose until there is
//	// no/a very small difference between successive averages
//	float norm;
//	int count = 0;
//	Matrix4 rotation = m;
//
//	do 
//	{
//		Matrix4 nextRotation;
//		Matrix4 currInvTranspose = rotation.Transposed().Inversed();
//
//		// Go through every component in the matrices and find the next matrix
//		for (int i = 0; i < 4; i++) 
//			for (int j = 0; j < 4; j++) 
//				nextRotation.m[i][j] = 0.5f * (rotation.m[i][j] + currInvTranspose.m[i][j]);
//
//		norm = 0.0f;
//		for (int i = 0; i < 3; i++) 
//		{
//			float n = 
//				fabs(rotation.m[i][0] - nextRotation.m[i][0]) +
//				fabs(rotation.m[i][1] - nextRotation.m[i][1]) +
//				fabs(rotation.m[i][2] - nextRotation.m[i][2]);
//			norm = max(norm, n);
//		}
//		rotation = nextRotation;
//	} 
//	while (count++ < 100 && norm > 0.000001f);
//
//	*outRotation = rotation;
//
//	// The scale is simply the removal of the rotation from the non-translated matrix
//	Matrix4 scaleMatrix = rotation.Inversed() * m;
//	
//	*outScale = Vector3(scaleMatrix.m[0][0], scaleMatrix.m[1][1], scaleMatrix.m[2][2]);
//}