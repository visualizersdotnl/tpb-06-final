
#include "Platform.h"
#include "Camera.h"
#include "Configuration.h"

namespace Pimp 
{
	Camera::Camera(World* ownerWorld)
		: Node(ownerWorld)
	{
		SetType(ET_Camera);
		SetFOVy(1.5f); // FIXME: this ain't 60 degrees. Why not?
	}

	void Camera::Tick(float timeDelta)
	{
		Node::Tick(timeDelta);
	}

	void Camera::Bind()
	{
		const Matrix4* mWorld = GetWorldTransform();
		worldPosition.x = mWorld->_41;
		worldPosition.y = mWorld->_42;
		worldPosition.z = mWorld->_43;

		CreateLookAt(&viewMatrix);

		viewProjMatrix = viewMatrix * projectionMatrix;
		viewMatrixInverse = viewMatrix.Inversed();

		ExtractFrustumPlanes();
	}

	void Camera::ExtractFrustumPlanes()
	{
		Matrix4& m = viewProjMatrix;

		// Calculate clipping planes in world space
		// Taken from: "Fast Extraction of Viewing Frustum Planes from the World-View-Projection Matrix"
		// by Gil Gribb and Klaus Hartmann (15/06/2001)
		
		frustumPlane[FRUSTUM_LEFT].a = m._14 + m._11;
		frustumPlane[FRUSTUM_LEFT].b = m._24 + m._21;
		frustumPlane[FRUSTUM_LEFT].c = m._34 + m._31;
		frustumPlane[FRUSTUM_LEFT].d = m._44 + m._41;

		frustumPlane[FRUSTUM_RIGHT].a = m._14 - m._11;
		frustumPlane[FRUSTUM_RIGHT].b = m._24 - m._21;
		frustumPlane[FRUSTUM_RIGHT].c = m._34 - m._31;
		frustumPlane[FRUSTUM_RIGHT].d = m._44 - m._41;

		frustumPlane[FRUSTUM_TOP].a = m._14 - m._12;
		frustumPlane[FRUSTUM_TOP].b = m._24 - m._22;
		frustumPlane[FRUSTUM_TOP].c = m._34 - m._32;
		frustumPlane[FRUSTUM_TOP].d = m._44 - m._42;

		frustumPlane[FRUSTUM_BOTTOM].a = m._14 + m._12;
		frustumPlane[FRUSTUM_BOTTOM].b = m._24 + m._22;
		frustumPlane[FRUSTUM_BOTTOM].c = m._34 + m._32;
		frustumPlane[FRUSTUM_BOTTOM].d = m._44 + m._42;

		frustumPlane[FRUSTUM_NEAR].a = m._13;
		frustumPlane[FRUSTUM_NEAR].b = m._23;
		frustumPlane[FRUSTUM_NEAR].c = m._33;
		frustumPlane[FRUSTUM_NEAR].d = m._43;

		frustumPlane[FRUSTUM_FAR].a = m._14 - m._13;
		frustumPlane[FRUSTUM_FAR].b = m._24 - m._23;
		frustumPlane[FRUSTUM_FAR].c = m._34 - m._33;
		frustumPlane[FRUSTUM_FAR].d = m._44 - m._43;

		// Normalize planes (only needed if we care about correct distances to the frustum planes)
		for (int iPlane = 0; iPlane < 6; ++iPlane)
			frustumPlane[iPlane].Normalize();
	}

	void Camera::SetFOVy(float FOVy)
	{
		this->FOVy = FOVy;

		// Recalculate projection matrix
		const float renderAspect = Configuration::Instance()->GetRenderAspectRatio();
		projectionMatrix = CreateMatrixPerspectiveFov(FOVy, renderAspect, 0.15f, 2000.0f);
	}
}
