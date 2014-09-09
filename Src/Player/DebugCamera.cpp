#include "DebugCamera.h"
#include "SceneTools.h"

DebugCamera::DebugCamera(Pimp::World* world) :
	isEnabled(false), world(world), isLookingAt(false)
{
	camera = new Pimp::Camera(world);
	world->GetElements().Add(camera);
	camera->SetFOVy(0.563197f);

	xform = new Pimp::Xform(world);
	world->GetElements().Add(xform);

	AddChildToParent(xform,world->GetRootNode());
	AddChildToParent(camera,xform);

	world->SetCurrentUserCamera(camera);
}


void DebugCamera::SetEnabled( bool enabled )
{
	if (enabled == isEnabled)
		return;

	isEnabled = enabled;

	world->SetUseCameraDirection(!isEnabled);

	if (isEnabled)
	{
		Pimp::Camera* prevDirectedCam = world->GetCurrentCamera();
		ASSERT(prevDirectedCam->GetParents().Size() == 1);
		Pimp::Node* prevDirectedCamParent = prevDirectedCam->GetParents()[0];
		ASSERT(prevDirectedCamParent->GetType() == Pimp::ET_Xform);
		Pimp::Xform* prevDirectedCamXform = (Pimp::Xform*)prevDirectedCamParent;

		xform->SetTranslation(prevDirectedCamXform->GetTranslation());
		xform->SetRotation(prevDirectedCamXform->GetRotation());
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

void DebugCamera::Roll(bool positive)
{
	Quaternion rot = xform->GetRotation();

	const float rollAmount = 0.10f; //< Totally framerate-dependent roll amount
	rot = CreateQuaternionFromYawPitchRoll(0, 0, positive ? rollAmount : -rollAmount) * rot;

	xform->SetRotation(rot);
}