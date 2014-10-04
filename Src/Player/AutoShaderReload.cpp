
#ifdef _DEBUG

#include "AutoShaderReload.h"
#include "Assets.h"

AutoShaderReload::AutoShaderReload( Pimp::World* world, float checkInterval )
	: world(world), checkInterval(checkInterval)
{
	FixedSizeList<Pimp::Scene*> scenes = world->GetScenes();

	for (int i=0; i<scenes.Size(); ++i)
	{
		Pimp::Scene* scene = scenes[i];

		if (scene->GetMaterial() != NULL && scene->GetMaterial()->GetShaderFileName().size() > 0)
		{
			ASSERT(FileExists(scene->GetMaterial()->GetShaderFileName())); // File should exist!
			
			SceneShaderFileChangeCheck entry;
			entry.scene = scene;
			entry.changeTracker = new FileChangeCheck(scene->GetMaterial()->GetShaderFileName().c_str());
			sceneShaderFiles.push_back(entry);
		}
	}
}


AutoShaderReload::~AutoShaderReload()
{
	for (int i=0; i<(int)sceneShaderFiles.size(); ++i)
		delete sceneShaderFiles[i].changeTracker;
}


void AutoShaderReload::Update()
{
	if (checkTimer.GetSecondsElapsed() >= checkInterval)
	{
		checkTimer.Reset();

		for (int i=0; i<(int)sceneShaderFiles.size(); ++i)
		{
			const SceneShaderFileChangeCheck& entry = sceneShaderFiles[i];

			if (entry.changeTracker->IsFileDirty())
			{
				ReloadSceneShader(entry.scene);
			}
		}
	}
}


void AutoShaderReload::ReloadSceneShader(Pimp::Scene* scene)
{
	Pimp::Material* oldMaterial = scene->GetMaterial();
	std::string shaderFileName = oldMaterial->GetShaderFileName();

	// Load shader file (text)
	unsigned char* shader_ascii;
	int shader_ascii_size;
	ReadFileContent(shaderFileName, &shader_ascii, &shader_ascii_size);

	// Compile our shader (text -> bytecode)
	unsigned char* compiled_shader;
	int compiled_shader_size;
	bool success = 
		Pimp::gD3D->CompileEffect(
		shader_ascii, 
		shader_ascii_size, 
		&compiled_shader, 
		&compiled_shader_size);

	if (success)
	{
		// Create new material
		Pimp::Material* newMaterial = new Pimp::Material(world, compiled_shader, compiled_shader_size, shaderFileName);
		scene->SetMaterial(newMaterial);
		world->GetMaterials().Add(newMaterial);

		// Clean up old material
		int oldIndex = world->GetMaterials().Find(oldMaterial);
		if (oldIndex >= 0)
			world->GetMaterials().RemoveAtIndex(oldIndex);

		delete oldMaterial;

		// Replace pointer in asset loader's administration
		Assets::ReplaceMaterial(oldMaterial, newMaterial);
	}
}

#endif