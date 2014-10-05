
#include <Core/Core.h>
#include <Shared/shared.h>
#include <LodePNG/lodepng.h>
#include "SceneTools.h"
#include "Assets.h"

// World, our Core container for the entire demo.
Pimp::World *gWorld = nullptr;

namespace Demo {

//
// Scene (part) base class.
// Primarily intended to manage all resource requests, objects and world manipulation for a single scene.
//

class Scene
{
public:
	Scene() :
		m_pScene(nullptr)
	{
	}

	virtual ~Scene() 
	{
		// @plek: As it seems, elements are released by the world's destructor.
//		delete m_pScene;
	}

	virtual void ReqAssets() = 0;           // Called prior to loading process: request assets only.
	virtual void BindAnimationNodes() = 0;  // Called during loading process: create & bind animation related nodes.
	virtual void BindAssets() = 0;          // Bind all resource based elements to the world (remember: asset loader takes care of it for assets).
	virtual void Tick() = 0;                // To be called from Demo::Tick().

protected:
	// Assuming that each part has at least one scene shader.
	Pimp::Scene *m_pScene;

	// Call this in BindToWorld() to bind the shader to the main scene.
	void SetSceneMaterial(Pimp::Material *pMat)
	{
		if (nullptr == m_pScene)
		{
			m_pScene = new Pimp::Scene(gWorld);
			gWorld->GetScenes().Add(m_pScene);
			gWorld->GetElements().Add(m_pScene);
		}

		m_pScene->SetMaterial(pMat);
	}
};

// Scenes:
#include "Ribbons.h"

//
// Asset root directory.
//

static const std::string GetAssetsPath()
{
	const std::string exePath = LowerCase(RemoveFilenameFromPath(GetCurrentProcessFileName()));
	return exePath + "assets\\";
}

//
// World generator & resource release.
//

static std::vector<Scene *> s_scenes;

bool GenerateWorld()
{
	gWorld = new Pimp::World();

	// We're now loading, but no disk I/O or anything else has taken place yet so it's an empty bar.
	DrawLoadProgress(0.f);

	// Set asset loader root path.
	Assets::SetRoot(GetAssetsPath());

	// Create scenes.
	s_scenes.push_back(new Ribbons());

	// Request resources.
	//

	for (Scene *pScene : s_scenes)
		pScene->ReqAssets();

	// Loaded some more! :)
	DrawLoadProgress(0.25f);

	// FIXME: Load other stuff here like the user post FX shader.

	// Let the asset loader do it's thing up to where all disk I/O is verified.
	if (false == Assets::StartLoading())
		return false;

	// Loaded some more! :)
	DrawLoadProgress(0.5f);

	// Bind animation related nodes (and do other CPU-based preparation work).
	//

	for (Scene *pScene : s_scenes)
		pScene->BindAnimationNodes();

	// Loaded some more! :)
	DrawLoadProgress(0.75f);

	// Wait for the asset loading to be all finished.
	if (false == Assets::FinishLoading())
		return false;

	// Setup resource related world objects.
	//

	for (Scene *pScene : s_scenes)
		pScene->BindAssets();

	// Ta-daa!
	DrawLoadProgress(1.f);

	// Finish up some World business.
	gWorld->InitAllBalls();
	gWorld->UpdateAllMaterialParameters();

	// Done!
	return true;
}

void ReleaseWorld()
{
	for (Scene *pScene : s_scenes)
		delete pScene;

	Assets::Release();

	delete gWorld;
	gWorld = nullptr;
}

// Tick function. 
// Here: manipulate the world and it's objects according to sync., prior to "ticking" & rendering it).
//

void Tick()
{
	s_scenes[0]->Tick();
}

} // namespace Demo
