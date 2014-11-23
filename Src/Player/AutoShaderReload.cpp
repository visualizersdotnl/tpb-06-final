
#if defined(_DEBUG) || defined(_DESIGN)

#include <Core/Platform.h>
#include <Core/Core.h>
#include "AutoShaderReload.h"
#include "Assets.h"

AutoShaderReload::AutoShaderReload( Pimp::World* world, float checkInterval )
	: world(world), checkInterval(checkInterval)
{
	const std::vector<Pimp::Scene*>& scenes = world->GetScenes();
	for (auto *scene : scenes)
	{
		ASSERT(nullptr != scene);

		Pimp::Material* material = scene->GetMaterial();
		if (nullptr != material && false == material->GetShaderFileName().empty())
		{
			// File should exist! (FIXME: ok, makes sense, but why exactly?)
			ASSERT(true == FileExists(material->GetShaderFileName()));

			Shader shader;
			shader.scene = scene;
			shader.changeTracker = new FileChangeCheck(scene->GetMaterial()->GetShaderFileName().c_str());
			shaders.push_back(shader);
		}
	}
}

AutoShaderReload::~AutoShaderReload()
{
	for (auto &shader : shaders)
		delete shader.changeTracker;
}

void AutoShaderReload::Update()
{
	if (checkTimer.GetSecondsElapsed() >= checkInterval)
	{
		checkTimer.Reset();

		for (auto &shader : shaders)
		{
			if (true == shader.changeTracker->HasChanged())
				ReloadSceneShader(shader.scene);
		}
	}
}

void AutoShaderReload::ReloadSceneShader(Pimp::Scene* scene)
{
	Pimp::Material* oldMaterial = scene->GetMaterial();
	std::string fileName = oldMaterial->GetShaderFileName();

	// Load shader file (text)
	unsigned char* shader_ascii = nullptr;
	int shader_ascii_size;
	ReadFileContent(fileName, &shader_ascii, &shader_ascii_size);

	// Compile our shader (text -> bytecode)
	unsigned char* compiled_shader;
	int compiled_shader_size;
	bool success = 
		Pimp::gD3D->CompileEffect(
		shader_ascii, 
		shader_ascii_size, 
		&compiled_shader, 
		&compiled_shader_size);

	delete[] shader_ascii;

	if (success)
	{
		std::vector<Pimp::Material*>& materials = world->GetMaterials();

		// Create new material
		Pimp::Material* newMaterial = new Pimp::Material(world, compiled_shader, compiled_shader_size, fileName);
		scene->SetMaterial(newMaterial);
		materials.push_back(newMaterial);

		// Clean up old material
		auto iMat = std::find(materials.begin(), materials.end(), oldMaterial);
		if (materials.end() != iMat)
			materials.erase(iMat);

		delete oldMaterial;

		// Replace pointer in asset loader's administration
		Assets::ReplaceMaterial(oldMaterial, newMaterial);
	}
}

#endif