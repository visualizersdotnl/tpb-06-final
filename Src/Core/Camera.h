#pragma once

#include "Node.h"

namespace Pimp 
{
	class Camera 
		: public Node
	{
	public:
		struct DOFSettings
		{
			DOFSettings(float depthNear, float depthFocus, float depthFar)
				: depthNear(depthNear), depthFocus(depthFocus), depthFar(depthFar)
			{}

			float depthNear;
			float depthFocus;
			float depthFar;			
		};

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

		DOFSettings settingsDOF;

		float FOVy;

		void ExtractFrustumPlanes();

	public:
		Camera(World* ownerWorld);
		virtual ~Camera() {}

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

		void SetDOFSettings(const DOFSettings& dof)
		{
			settingsDOF = dof;
		}

		const DOFSettings& GetDOFSettings() const
		{
			return settingsDOF;
		}
	};
}