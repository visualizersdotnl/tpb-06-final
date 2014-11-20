
#pragma once

// #include <Core/Core.h>

class DebugCamera
{
public:
	DebugCamera(Pimp::World* world);
	~DebugCamera() {}

	void SetEnabled(bool enabled);

	// Mouse control.	
	void StartLookAt();
	void EndLookAt();
	void LookAt(int deltaMouseX, int deltaMouseY);

	// Keyb. control.
	void Move(const Vector3& directionViewSpace);	
	void Roll(bool positive);

	void DumpCurrentTransformToOutputWindow();

	Pimp::Camera *Get() const { return camera; }

private:
	Pimp::World* world;

	bool isEnabled;
	Pimp::Camera* camera;
	Pimp::Xform* xform;

	bool isLookingAt;
	Quaternion lookAtInitialRotation;
};
