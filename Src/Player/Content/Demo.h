
#pragma once

// #include <string>
#include <Core/Core.h>

namespace Demo
{
	const std::string GetAssetsPath();

	bool GenerateWorld(const char *rocketClient);
	void ReleaseWorld();

	bool Tick(Pimp::Camera *camOverride);
	void WorldRender();
}
