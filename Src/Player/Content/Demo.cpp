
#include <vector>
#include <Core/Core.h>
#include <Shared/shared.h>
#include <LodePNG/lodepng.h>
#include "SceneTools.h"
#include "Assets.h"
#include "Audio.h"
#include "Settings.h"

// World, our Core container for the entire demo.
Pimp::World *gWorld = nullptr;

// Helper function(s).
//

inline DWORD AlphaToVtxColor(float alpha)
{
	const unsigned char iAlpha = int(alpha*255.f);
	return iAlpha<<24|0xffffff;
}

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
	SyncTrack(const std::string &name, bool uploadForShaders, const sync_track **ppRef = nullptr) :
		m_track(sync_get_track(s_Rocket, name.c_str())),
		m_name(name) 
	{
		ASSERT(nullptr != m_track);

		if (true == uploadForShaders)
			matParam = CreateShaderParamFromTrack(name);
		else
			matParam = nullptr;

		if (nullptr != ppRef)
			*ppRef = m_track;
	}

	float Get(double row) const 
	{ 
		return (float) sync_get_val(m_track, row); 
	}

	// Call each frame!
	void Update(double row)
	{
		if (nullptr != matParam)
			matParam->SetValue(Get(row));
	}

	const sync_track *m_track;
	const std::string m_name;
	Pimp::MaterialParameter *matParam;
};

static std::vector<SyncTrack> syncTracks;

// Sync. references:

// global
static const sync_track *st_SceneIdx;
static const sync_track *st_fxTimeGlobal;
static const sync_track *st_postFadeInOut;
static const sync_track *st_defRotX, *st_defRotY, *st_defRotZ, *st_defRotW;
static const sync_track *st_defTransX, *st_defTransY, *st_defTransZ;

// part: James Bond intro
static const sync_track *st_bondBlob1;
static const sync_track *st_bondBlobFade1;
static const sync_track *st_bondBlob2;
static const sync_track *st_bondBlobFade2;
static const sync_track *st_bondAmpFade;
static const sync_track *st_bondGroupsFade;
static const sync_track *st_bondPresFade;
static const sync_track *st_bondWhite;
static const sync_track *st_bondTarget;
static const sync_track *st_bondSoundFX;
static const sync_track *st_bondPimpFade;

void CreateRocketTracks()
{
	syncTracks.clear();

	// GLOBAL TRACKS
	syncTracks.push_back(SyncTrack("SceneIndex", false, &st_SceneIdx)); 
	syncTracks.push_back(SyncTrack("fxTimeGlobal", true, &st_fxTimeGlobal));
	syncTracks.push_back(SyncTrack("fadeInOut", true, &st_postFadeInOut)); 
	syncTracks.push_back(SyncTrack("defRotQuat_X", false, &st_defRotX));
	syncTracks.push_back(SyncTrack("defRotQuat_y", false, &st_defRotY));
	syncTracks.push_back(SyncTrack("defRotQuat_Z", false, &st_defRotZ));
	syncTracks.push_back(SyncTrack("defRotQuat_W", false, &st_defRotW));
	syncTracks.push_back(SyncTrack("defTrans_X", false, &st_defTransX));
	syncTracks.push_back(SyncTrack("defTrans_Y", false, &st_defTransY));
	syncTracks.push_back(SyncTrack("defTrans_Z", false, &st_defTransZ));

	// FX ONLY TRACKS (non-indexed, don't care as long as they're updated)
	//

	// KNOT
	syncTracks.push_back(SyncTrack("knotTubeRadius1", true));
	syncTracks.push_back(SyncTrack("knotTubeRadius2", true));
	syncTracks.push_back(SyncTrack("knotTubeRadius3", true));
	
	// BONDTRO
	syncTracks.push_back(SyncTrack("bondBlob1", false, &st_bondBlob1));
	syncTracks.push_back(SyncTrack("bondBlobFade1", false, &st_bondBlobFade1));
	syncTracks.push_back(SyncTrack("bondBlob2", false, &st_bondBlob2));
	syncTracks.push_back(SyncTrack("bondBlobFade2", false, &st_bondBlobFade2));
	syncTracks.push_back(SyncTrack("bondAmpFade", false, &st_bondAmpFade));
	syncTracks.push_back(SyncTrack("bondGroupsFade", false, &st_bondGroupsFade));
	syncTracks.push_back(SyncTrack("bondPresFade", false, &st_bondPresFade));
	syncTracks.push_back(SyncTrack("bondWhite", false, &st_bondWhite));
	syncTracks.push_back(SyncTrack("bondTarget", false, &st_bondTarget));
	syncTracks.push_back(SyncTrack("bondSoundFX", false, &st_bondSoundFX));
	syncTracks.push_back(SyncTrack("bondPimpFade", false, &st_bondPimpFade));
}

 namespace Demo {

 //
 // Shared objects.
 //

 // Default (static) world to camera transformation.
static Pimp::Camera *s_defaultCam;
static Pimp::Xform *s_defaultXform;

// White texture (for untextured sprites).
static Pimp::Texture2D *texWhite;

// 2D sprite batcher.
static Pimp::Sprites *s_sprites = nullptr;

// Global material(s).
static Pimp::Material *matUserPostFX;
 
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
	virtual void Tick(double row) = 0;      // To be called from Demo::Tick().

protected:
	// Assuming that each part has at least one scene shader.
	Pimp::Scene *m_pScene;
	int m_sceneIdx;

	// Call this in BindToWorld() to bind the shader to the main scene.
	void BindSceneMaterial(Pimp::Material *pMat)
	{
		if (nullptr == m_pScene)
		{
			m_pScene = new Pimp::Scene(gWorld);
			gWorld->GetScenes().Add(m_pScene);
			gWorld->GetElements().Add(m_pScene);
			m_sceneIdx = gWorld->GetScenes().Size()-1;	

			m_pScene->SetMaterial(pMat);
		}
	}

	void SetSceneMaterial()
	{
		// @plek: Why is this shit here again?
	}

	// And this one on top of Tick() to activate said scene.
	void SetMainSceneAndDefaultCamera() 
	{ 
		gWorld->SetCurrentUserCamera(s_defaultCam);
		gWorld->SetCurrentSceneIndex(m_sceneIdx); 
	}
};

// Scenes:
#include "Bondtro.h"
#include "Ribbons.h"
#include "Knot.h"

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

static std::vector<Demo::Scene *> s_scenes;

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
	s_scenes.push_back(new Bondtro());
	s_scenes.push_back(new Ribbons());
//	s_scenes.push_back(new Knot());

	// Request resources.
	//

	texWhite = Pimp::gD3D->GetWhiteTex();

	for (Scene *pScene : s_scenes)
		pScene->ReqAssets();

	// Loaded some more! :)
	DrawLoadProgress(0.25f);

	// Req. global stuff.
	Assets::AddMaterial("shaders\\posteffect.fx", &matUserPostFX);

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
//	s_defaultXform->SetTranslation(Vector3(0.f, 0.f, 4.f));
	AddChildToParent(s_defaultXform, gWorld->GetRootNode());
	AddChildToParent(s_defaultCam, s_defaultXform);
	s_defaultXform->SetTranslation(Vector3(0.f, 0.f, 4.f));

	// Sprite batcher.
	s_sprites = new Pimp::Sprites();

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

	// Add user postFX.
	gWorld->GetMaterials().Add(matUserPostFX);
	gWorld->GetPostProcess()->SetUserPostEffect(matUserPostFX);

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
	delete s_sprites;

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

static float clearR = 0.f, clearG = 0.f, clearB = 0.f;

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

//	clearR = (float)sync_get_val(st_clearR, rocketRow);
//	clearG = (float)sync_get_val(st_clearG, rocketRow);
//	clearB = (float)sync_get_val(st_clearB, rocketRow);

	// update default camera
	const float defCamTrans_X = (float) sync_get_val(st_defTransX, rocketRow);
	const float defCamTrans_Y = (float) sync_get_val(st_defTransY, rocketRow);
	const float defCamTrans_Z = (float) sync_get_val(st_defTransZ, rocketRow);
	const float defCamRotQuat_X = (float) sync_get_val(st_defRotX, rocketRow);
	const float defCamRotQuat_Y = (float) sync_get_val(st_defRotY, rocketRow);
	const float defCamRotQuat_Z = (float) sync_get_val(st_defRotZ, rocketRow);
	const float defCamRotQuat_W = (float) sync_get_val(st_defRotW, rocketRow);
	s_defaultXform->SetTranslation(Vector3(defCamTrans_X, defCamTrans_Y, defCamTrans_Z));
	s_defaultXform->SetRotation(Quaternion(defCamRotQuat_X, defCamRotQuat_Y, defCamRotQuat_Z, defCamRotQuat_W));

	const int sceneIdx = (int) sync_get_val(st_SceneIdx, rocketRow);
//	const int sceneIdx = 0; // For test!
	if (-1 != sceneIdx)
		s_scenes[sceneIdx]->Tick(rocketRow);
	else
		return false; // Demo is finished.

	// This is primarily used to feed the debug camera if need be.
	if (nullptr != camOverride)
	{
		gWorld->SetCurrentUserCamera(camOverride);
	}

	return true;
}

void WorldRender() {
	gWorld->Render(clearR, clearG, clearB, s_sprites); }


} // namespace Demo
