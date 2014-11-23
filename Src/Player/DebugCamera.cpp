
#include <Core/Platform.h>
#include <Shared/misc.h>
#include <Core/Core.h>
#include "DebugCamera.h"
#include "SceneTools.h"

DebugCamera::DebugCamera(Pimp::World* world) :
	world(world)
,	isEnabled(false)
,	isLookingAt(false)
{
	ASSERT(nullptr != world);

	camera = new Pimp::Camera(world);
	world->GetElements().push_back(camera);
	camera->SetFOVy(0.563197f);

	xform = new Pimp::Xform(world);
	world->GetElements().push_back(xform);

	AddChildToParent(xform,world->GetRootNode());
	AddChildToParent(camera,xform);
}

void DebugCamera::SetEnabled( bool enabled )
{
	if (enabled == isEnabled)
		return;
	else
	{
		isEnabled = enabled;

		if (true == isEnabled)
		{
			// Adopt current camera.
			Pimp::Camera* prevCam = world->GetCamera();
			ASSERT(prevCam->GetParents().size() == 1);
			Pimp::Node* prevCamParent = prevCam->GetParents()[0];
			ASSERT(prevCamParent->GetType() == Pimp::ET_Xform);
			Pimp::Xform* prevDirectedCamXform = static_cast<Pimp::Xform*>(prevCamParent);

			// And then set it as ours.
			xform->SetTranslation(prevDirectedCamXform->GetTranslation());
			xform->SetRotation(prevDirectedCamXform->GetRotation());
			world->SetCamera(camera);
		}
	}
}

void DebugCamera::Move( const Vector3& directionViewSpace )
{
	float speed = 1.0f; //< Totally framerate-dependent movement speed

	Vector3 dirWorld = xform->GetWorldTransform()->TransformNormal(directionViewSpace);

	Vector3 pos = xform->GetTranslation();
	pos += dirWorld * speed;
	xform->SetTranslation(pos);
}

void DebugCamera::Roll(bool positive)
{
	Quaternion rot = xform->GetRotation();

	const float rollAmount = 0.10f; //< Totally framerate-dependent roll amount
	rot = CreateQuaternionFromYawPitchRoll(0, 0, positive ? rollAmount : -rollAmount) * rot;

	xform->SetRotation(rot);
}

void DebugCamera::StartLookAt()
{
	ASSERT(!isLookingAt);
	isLookingAt = true;

	lookAtInitialRotation = xform->GetRotation();
}

void DebugCamera::EndLookAt()
{
	ASSERT(isLookingAt);
	isLookingAt = false;
}

void DebugCamera::LookAt(int deltaMouseX, int deltaMouseY)
{
	ASSERT(isLookingAt);

	// Calculate new orientation
	const float mouseSensitivity = -0.01f;

	float yaw = deltaMouseX * mouseSensitivity;
	float pitch = deltaMouseY * mouseSensitivity;

	Quaternion camOrientationDelta = CreateQuaternionFromYawPitchRoll(yaw, pitch, 0);

	Quaternion newRot = camOrientationDelta * lookAtInitialRotation;

	xform->SetRotation(newRot);
}

void DebugCamera::DumpCurrentTransformToOutputWindow()
{
	Quaternion rot = xform->GetRotation();
	Vector3 pos = xform->GetTranslation();

	Vector3 rotEulerXYZ = rot.GetEulerAnglesXYZ();

	DEBUG_LOG("Current debug camera transform:");
	DEBUG_LOG("X = %.2ff", pos.x);
	DEBUG_LOG("Y = %.2ff", pos.y);
	DEBUG_LOG("Z = %.2ff", pos.z);
	DEBUG_LOG("X = %.2ff", rot.x);
	DEBUG_LOG("Y = %.2ff", rot.y);
	DEBUG_LOG("Z = %.2ff", rot.z);
	DEBUG_LOG("W = %.2ff", rot.w);
}
