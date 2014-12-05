
// Helper class that monitors shader file changes, triggers a recompile and swaps the materials (which contain the shader).
// FIXME: this isn't pretty (also see Core::Scene) so I'd like to review it.

#pragma once

#if defined(_DEBUG) || defined(_DESIGN)

// #include <Core/Core.h>
#include <Shared/stopwatch.h>
#include <Shared/fileutils.h>

class AutoShaderReload : public boost::noncopyable
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