#include "Camera.h"
#include "D3D.h"
#include "Configuration.h"

namespace Pimp 
{
	Camera::Camera(World* ownerWorld)
		: Node(ownerWorld)
	{
		SetType(ET_Camera);

		SetFOVy(1.5f);
	}

#ifdef _DEBUG
	Camera::~Camera()
	{

	}
#endif

	void Camera::Tick(float timeDelta)
	{

		Node::Tick(timeDelta);
	}


	void Camera::Bind()
	{
		const Matrix4* wurld = GetWorldTransform();

		CreateLookAt(&viewMatrix);

		worldPosition.x = wurld->_41;
		worldPosition.y = wurld->_42;
		worldPosition.z = wurld->_43;

		// calc viewproj
		viewProjMatrix = viewMatrix * projectionMatrix;

		// calc viewinv
		viewMatrixInverse = viewMatrix.Inversed();

		// calc our planes... used for frustum culling
		ExtractFrustumPlanes();
	}

	void Camera::ExtractFrustumPlanes()
	{
		Matrix4& m = viewProjMatrix;

		// calculate clipping planes in worldspace
		// lots of kudos to the paper 
		// "Fast Extraction of Viewing Frustum Planes from the World-View-Projection Matrix"
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

		// normalize planes
		// (only needed if we care about correct distances to the frustum planes)
		for (int i=0; i<6; ++i)
			frustumPlane[i].Normalize();

	}

	void Camera::SetFOVy(float FOVy)
	{
		this->FOVy = FOVy;

		// recalc projection matrix
		float renderAspect = Configuration::Instance()->GetMaxDisplayAspectRatio().aspect;
		projectionMatrix = CreateMatrixPerspectiveFov(FOVy, renderAspect, 0.15f, 2000.0f);
	}
}