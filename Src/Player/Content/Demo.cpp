
#include <Core/Core.h>
#include <Shared/shared.h>
#include <LodePNG/lodepng.h>
#include "SceneTools.h"
#include "Assets.h"
#include "Audio.h"

// World, our Core container for the entire demo.
Pimp::World *gWorld = nullptr;

//
// Rocket stuff.
//
// Rocket can run in edit mode (requires the client (sync_player.exe) to be connected) or
// it can run from exported data in replay mode.
//
// Right now, Player has 3 different build configurations:
// -   Debug: debug build that runs Rocket in edit mode.
// -  Design: release build that runs Rocket in edit mode.
// - Release: release build that runs Rocket in replay mode (globally defines SYNC_PLAYER).
//

// Library required for Rocket.
#pragma comment(lib, "ws2_32.lib")

#include "../../Libs/rocket/lib/sync.h"

#if !defined (SYNC_PLAYER)

// Audio player hooks.
static sync_cb s_rocketHooks = 
{ 
	Audio_Rocket_Pause, 
	Audio_Rocket_SetRow, 
	Audio_Rocket_IsPlaying 
};

#endif

static sync_device *s_Rocket = nullptr;

// All tracks (they do not have to be freed individually):
// ...

void CreateRocketTracks()
{
}
 

 namespace Demo {

//
// Scene (part) base class.
// Primarily intended to manage all resource requests, objects and world manipulation for a single scene.
//
// !! IMPORTANT !!
// All objects added as an element to the world are destroyed by the world itself!
// This includes stuff like scenes and cameras!
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

const std::string GetAssetsPath()
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

	// Try to fire up Rocket.
	s_Rocket = sync_create_device("assets\\sync");
	if (nullptr == s_Rocket)
	{
		SetLastError("Unable to start GNU Rocket.");
		return false;
	}

	// Instantiate all Rocket tracks.
	CreateRocketTracks();

#if !defined(SYNC_PLAYER)
	// We're in edit mode: connect to Rocket client.
	if (0 != sync_connect(s_Rocket, "localhost", SYNC_DEFAULT_PORT))
	{
		SetLastError("Unable to connect to a GNU Rocket client.");
		return false;
	}
#endif

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

#if defined(SYNC_PLAYER)
	// We're in replay mode so start the soundtrack, otherwise Rocket will tell use what to do.
	Audio_Start();
#endif

	// Done!
	return true;
}

void ReleaseWorld()
{
	for (Scene *pScene : s_scenes)
		delete pScene;

	Assets::Release();

	if (nullptr != s_Rocket)
		sync_destroy_device(s_Rocket);

	delete gWorld;
	gWorld = nullptr;
}

//
// Tick function. 
// Here: manipulate the world and it's objects according to sync., *prior* to "ticking" & rendering it).
//

bool Tick()
{
	const double rocketRow = Audio_Rocket_GetRow();

#if !defined(SYNC_PLAYER)
	if (0 != sync_update(s_Rocket, int(floor(rocketRow)), &s_rocketHooks, nullptr))
	{
		SetLastError("Connection to GNU Rocket lost!");
		return false;
	}

	// If we're paused (and probably scrolling around and such), keep telling World where we're at 
	// in time.
	if (0 == Audio_Rocket_IsPlaying(nullptr))
		gWorld->ForceSetTime(Audio_GetPos()); // FIXME: GetPos() doesn't work if channel isn't playing: derive from row.
#endif

	s_scenes[0]->Tick();

	// FIXME: return false if demo is done.
	return true;
}

} // namespace Demo
