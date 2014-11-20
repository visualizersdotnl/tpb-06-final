
#pragma once

#include <Core/Core.h>

namespace Demo
{
	const std::string GetAssetsPath();

	bool GenerateWorld(const char *rocketClient); // Hostname to connect to.
	void ReleaseWorld();

	bool Tick(float timeElapsed, Pimp::Camera *pDebugCam = nullptr);
	void WorldRender();

	Pimp::World *GetWorld();
}
