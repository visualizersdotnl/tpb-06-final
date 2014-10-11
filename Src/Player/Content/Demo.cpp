
#include <Core/Core.h>
#include <Shared/shared.h>
#include <LodePNG/lodepng.h>
#include "SceneTools.h"
#include "Assets.h"
#include "Audio.h"
#include "Settings.h"

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

double kRocketRowRate = (PIMPPLAYER_ROCKET_BPM/60.0) * PIMPPLAYER_ROCKET_RPB;

void Rocket_Pause(void *, int bPause)
{
	if (0 == bPause)
		Audio_Unpause();
	else
		Audio_Pause();
}

void Rocket_SetRow(void *, int row)
{
	const double secPos = floor(row / kRocketRowRate);
	Audio_SetPosition((float) secPos);
}

int Rocket_IsPlaying(void *)
{
	return Audio_IsPlaying();
}

double Rocket_GetRow()
{
	return floor(Audio_GetPosition()*kRocketRowRate);
}

// Audio player hooks.
static sync_cb s_rocketHooks = 
{ 
	Rocket_Pause, 
	Rocket_SetRow, 
	Rocket_IsPlaying 
};

#endif

static sync_device *s_Rocket = nullptr;

static Pimp::MaterialParameter *CreateShaderParamFromTrack(const std::string &name) // Element, so no need to release.
{
	Pimp::MaterialParameter *newParam = new Pimp::MaterialParameter(gWorld);
	gWorld->GetElements().Add(newParam);
	newParam->SetValueType(Pimp::MaterialParameter::VT_Value);
	newParam->SetName(name.c_str());
	return newParam;
}

// @plek: Note, Rocket tracks do not have to be released individually, neither does my container.
class SyncTrack
{
public:
	SyncTrack(const std::string &name) :
		m_track(sync_get_track(s_Rocket, name.c_str())),
		m_name(name) 
	{
		ASSERT(nullptr != m_track);
		matParam = CreateShaderParamFromTrack(name);
	}

	float Get(double row) const 
	{ 
		return (float) sync_get_val(m_track, row); 
	}

	// Call each frame!
	void Update(double row)
	{
		matParam->SetValue(Get(row));
	}

	const sync_track *m_track;
	const std::string m_name;
	Pimp::MaterialParameter *matParam;
};

static std::vector<SyncTrack> syncTracks;

const unsigned int kSync_SceneIdx = 0;
const unsigned int kSync_fxTimeGlobal = 1;

void CreateRocketTracks()
{
	syncTracks.clear();

	// GLOBAL TRACKS (indexed, keep in line with the crap above)
	syncTracks.push_back(SyncTrack("SceneIndex")); 
	syncTracks.push_back(SyncTrack("fxTimeGlobal"));

	// SHADER ONLY TRACKS (non-indexed, don't care as long as they're updated)
	// ...
}

 namespace Demo {

 //
 // Shared objects.
 //

 // Default (static) world to camera transformation.
static Pimp::Camera *s_defaultCam;
static Pimp::Xform *s_defaultXform;
 
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
		m_pScene(nullptr),
		m_sceneIdx(-1)
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
	int m_sceneIdx;

	// Call this in BindToWorld() to bind the shader to the main scene.
	void SetSceneMaterial(Pimp::Material *pMat)
	{
		if (nullptr == m_pScene)
		{
			m_pScene = new Pimp::Scene(gWorld);
			gWorld->GetScenes().Add(m_pScene);
			gWorld->GetElements().Add(m_pScene);
			m_sceneIdx = gWorld->GetElements().Size()-1;		
		}

		m_pScene->SetMaterial(pMat);
	}

	// And this one on top of Tick() to activate said scene.
	void SetMainSceneAndDefaultCamera() 
	{ 
		gWorld->SetCurrentUserCamera(s_defaultCam);
		gWorld->SetCurrentSceneIndex(m_sceneIdx); 
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

bool GenerateWorld(const char *rocketClient)
{
	gWorld = new Pimp::World();

	const std::string assetsPath = GetAssetsPath();
	const std::string syncPath = assetsPath + "sync\\";

	// Try to fire up Rocket.
	s_Rocket = sync_create_device(syncPath.c_str());
	if (nullptr == s_Rocket)
	{
		SetLastError("Unable to start GNU Rocket.");
		return false;
	}

		// Instantiate all Rocket tracks.
	CreateRocketTracks();

#if !defined(SYNC_PLAYER)
	// We're in edit mode: connect to Rocket client.
	if (0 != sync_connect(s_Rocket, rocketClient, SYNC_DEFAULT_PORT))
	{
		SetLastError("Unable to connect to a GNU Rocket client.");
		return false;
	}
#endif

	// We're now loading, but no disk I/O or anything else has taken place yet so it's an empty bar.
	DrawLoadProgress(0.f);

	// Set asset loader root path.
	Assets::SetRoot(assetsPath);

	// Create scenes.
	s_scenes.push_back(new Ribbons());

	// Request resources.
	//

	for (Scene *pScene : s_scenes)
		pScene->ReqAssets();

	// Loaded some more! :)
	DrawLoadProgress(0.25f);

	// FIXME: Request other stuff here like the user post FX shader.

	// Let the asset loader do it's thing up to where all disk I/O is verified.
	if (false == Assets::StartLoading())
		return false;

	// Loaded some more! :)
	DrawLoadProgress(0.5f);

	// Bind animation related nodes (and do other CPU-based preparation work).
	//

	// Add (static) default transformation.
	s_defaultCam = new Pimp::Camera(gWorld);
	gWorld->GetElements().Add(s_defaultCam);
	s_defaultCam->SetFOVy(0.563197f);
	s_defaultXform = new Pimp::Xform(gWorld);
	gWorld->GetElements().Add(s_defaultXform);
	s_defaultXform->SetTranslation(Vector3(0.f, 0.f, 4.f));
	AddChildToParent(s_defaultXform, gWorld->GetRootNode());
	AddChildToParent(s_defaultCam, s_defaultXform);
	s_defaultXform->SetTranslation(Vector3(0.f, 0.f, 4.f));

	for (Scene *pScene : s_scenes)
		pScene->BindAnimationNodes();

	// We're using Rocket- or debug-driven cameras only!
	gWorld->SetCurrentUserCamera(s_defaultCam);
	gWorld->SetUseCameraDirection(false);

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

bool Tick(Pimp::Camera *camOverride)
{
	const double rocketRow = Rocket_GetRow();

#if !defined(SYNC_PLAYER)
	if (0 != sync_update(s_Rocket, int(floor(rocketRow)), &s_rocketHooks, nullptr))
	{
		SetLastError("Connection to GNU Rocket lost!");
		return false;
	}
#endif

	// Update tracks.
	for (SyncTrack &syncTrack : syncTracks)
		syncTrack.Update(rocketRow);

	const int sceneIdx = (int) syncTracks[kSync_SceneIdx].Get(rocketRow);
	if (-1 != sceneIdx)
		s_scenes[sceneIdx]->Tick();
	else
		return false; // Demo is finished.

	// This is primarily used to feed the debug camera if need be.
	if (nullptr != camOverride)
	{
		gWorld->SetCurrentUserCamera(camOverride);
	}

	return true;
}

} // namespace Demo
