#pragma once

#ifdef _DEBUG

#include <Core/Core.h>
#include <Shared/shared.h>

// Helper class that can monitor file changes to shader files and automatically trigger recompilation when changes
// were detected.
class AutoShaderReload
{
private:
	Pimp::World* world;
	Stopwatch checkTimer;
	float checkInterval;
	
	struct SceneShaderFileChangeCheck
	{
		Pimp::Scene* scene;
		FileChangeCheck* changeTracker;
	};
	std::vector<SceneShaderFileChangeCheck> sceneShaderFiles;

	void ReloadSceneShader(Pimp::Scene* scene);

public:
	AutoShaderReload(Pimp::World* world, float checkInterval);
	~AutoShaderReload();

	void Update();
};

#endif