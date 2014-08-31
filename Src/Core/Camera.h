#pragma once

#include "Node.h"

namespace Pimp 
{
	class Camera 
		: public Node
	{
	protected:
		enum FrustumPlane
		{
			FRUSTUM_LEFT = 0,
			FRUSTUM_RIGHT,
			FRUSTUM_TOP,
			FRUSTUM_BOTTOM,
			FRUSTUM_NEAR,
			FRUSTUM_FAR
		};

		Matrix4 viewMatrix;
		Matrix4 viewMatrixInverse;
		Matrix4 viewProjMatrix;
		Matrix4 projectionMatrix;

		Vector3 worldPosition;
		Vector3 cameraNearSize;

		Plane frustumPlane[6];

		float FOVy;

		void ExtractFrustumPlanes();

	public:
		Camera(World* ownerWorld);
#ifdef _DEBUG
		virtual ~Camera();
#endif

		virtual void Tick(float timeDelta);
		
		void Bind();

		void SetFOVy(float FOVy);

		const Matrix4* GetViewProjectionMatrixPtr() const 
		{
			return &viewProjMatrix;
		}

		const Matrix4* GetViewMatrixPtr() const 
		{
			return &viewMatrix;
		}

		const Matrix4* GetViewInvMatrixPtr() const 
		{
			return &viewMatrixInverse;
		}

		const Vector3& GetWorldPosition() const 
		{
			return worldPosition;
		}
	};
}