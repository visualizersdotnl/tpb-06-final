#pragma once

#include <Core/Core.h>

class DebugCamera
{
public:
	DebugCamera(Pimp::World* world);

	void SetEnabled(bool enabled);

	void Move(const Vector3& directionViewSpace);	
	
	void StartLookAt();
	void EndLookAt();
	void LookAt(int deltaMouseX, int deltaMouseY);

	void Roll(bool positive);

	void DumpCurrentTransformToOutputWindow();

	// @plek: Let the client allow where and how to apply it.
	Pimp::Camera *Get() const { return camera; }

private:
	bool isEnabled;

	Pimp::World* world;
	Pimp::Camera* camera;
	Pimp::Xform* xform;

	bool isLookingAt;
	Quaternion lookAtInitialRotation;
};
