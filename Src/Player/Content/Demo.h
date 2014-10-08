
#pragma once

// #include <string>

namespace Demo
{
	const std::string GetAssetsPath();

	bool GenerateWorld(const char *rocketClient);
	void ReleaseWorld();

	bool Tick(Pimp::Camera *camOverride);
}
