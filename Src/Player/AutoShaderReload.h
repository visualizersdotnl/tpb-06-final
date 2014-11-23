#pragma once

#if defined(_DEBUG) || defined(_DESIGN)

// #include <Core/Core.h>
#include <Shared/stopwatch.h>
#include <Shared/fileutils.h>

// Helper class that can monitor file changes to shader files and automatically trigger recompilation when changes
// were detected.
class AutoShaderReload
{
private:
	Pimp::World* world;
	Stopwatch checkTimer;
	float checkInterval;
	
	struct Shader
	{
		Pimp::Scene* scene;
		FileChangeCheck* changeTracker;
	};
	std::vector<Shader> shaders;

	void ReloadSceneShader(Pimp::Scene* scene);

public:
	AutoShaderReload(Pimp::World* world, float checkInterval);
	~AutoShaderReload();

	void Update();
};

#endif