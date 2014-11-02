
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

// Misc. helper stuff.
//

inline DWORD AlphaToVtxColor(float alpha, unsigned int RGB = 0xffffff)
{
	const unsigned char iAlpha = int(alpha*255.f);
	return iAlpha<<24|RGB;
}

const float kTileMul = (PIMPPLAYER_RENDER_ASPECT_RATIO);

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

#include "../../Libs/rocket/lib/sync.h"

double kRocketRowRate = (PIMPPLAYER_ROCKET_BPM/60.0) * PIMPPLAYER_ROCKET_RPB;

double Rocket_GetRow()
{
	return floor(Audio_GetPosition()*kRocketRowRate);
}

#if !defined (SYNC_PLAYER)

// Library required for Rocket.
#pragma comment(lib, "ws2_32.lib")

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

static std::vector<SyncTrack> s_syncTracks;

// Global sync.
//

static const sync_track *st_SceneIdx;
static const sync_track *st_fxTime;
static const sync_track *st_postFlash ,*st_postFade;
static const sync_track *st_sceneFadeInOut;
static const sync_track *st_sceneNoise, *st_sceneNoiseT;
static const sync_track *st_defRotX, *st_defRotY, *st_defRotZ, *st_defRotW;
static const sync_track *st_defTransX, *st_defTransY, *st_defTransZ;

void CreateGlobalRocketTracks()
{
	s_syncTracks.clear();

	s_syncTracks.push_back(SyncTrack("g_SceneIndex", false, &st_SceneIdx)); 
	s_syncTracks.push_back(SyncTrack("g_fxTime", true, &st_fxTime));
	s_syncTracks.push_back(SyncTrack("g_postFlash", true, &st_postFlash));
	s_syncTracks.push_back(SyncTrack("g_postFade", true, &st_postFade));
	s_syncTracks.push_back(SyncTrack("g_preSpriteFade", true, &st_sceneFadeInOut)); 
	s_syncTracks.push_back(SyncTrack("g_sceneNoise", true, &st_sceneNoise)); 
	s_syncTracks.push_back(SyncTrack("g_sceneNoiseT", true, &st_sceneNoiseT)); 
	s_syncTracks.push_back(SyncTrack("g_defRotQuat_X", false, &st_defRotX));
	s_syncTracks.push_back(SyncTrack("g_defRotQuat_y", false, &st_defRotY));
	s_syncTracks.push_back(SyncTrack("g_defRotQuat_Z", false, &st_defRotZ));
	s_syncTracks.push_back(SyncTrack("g_defRotQuat_W", false, &st_defRotW));
	s_syncTracks.push_back(SyncTrack("g_defTrans_X", false, &st_defTransX));
	s_syncTracks.push_back(SyncTrack("g_defTrans_Y", false, &st_defTransY));
	s_syncTracks.push_back(SyncTrack("g_defTrans_Z", false, &st_defTransZ));
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

// Single metaballs instance (see .cpp for explanation).
static Pimp::Metaballs *s_pMetaballs = nullptr;
 
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

	virtual void ReqRocketTracks() = 0;     // Called after Rocket is fired up. Request your private tracks here.
	                                        // ^^ Don't forget there's a global amount of tracks like g_fxTimer!
	virtual void ReqAssets() = 0;           // Called prior to loading process: request assets only.
	virtual void BindToWorld() = 0;         // Bind and create elements to the world (remember: asset loader takes care of it for assets).
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

	// Helper to bind a material parameter to Xform (regular or inverse) and add it to the world.
	static Pimp::MaterialParameter* AddMaterialParamWithXform(const char* name, bool inverse)
	{
		Pimp::Xform* xform = new Pimp::Xform(gWorld);
		
		Pimp::MaterialParameter* param = new Pimp::MaterialParameter(gWorld);
		gWorld->GetElements().Add(param);
		
		if (true ==  inverse)
			param->SetValueType(Pimp::MaterialParameter::VT_NodeXformInv);
		else
			param->SetValueType(Pimp::MaterialParameter::VT_NodeXform);

		param->SetName(name);

		Pimp::World::StaticAddChildToParent(xform, gWorld->GetRootNode());
		Pimp::World::StaticAddChildToParent(param, xform);

		return param;
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
#include "GeneralCinema.h"
#include "Shafts.h"
#include "Ribbons2.h"
#include "Pompom.h"

// Shared statics.
static const unsigned int kNumMetaball4s = 14;
static __declspec(align(16)) Pimp::Metaballs::Metaball4 s_metaball4s[kNumMetaball4s];
#include "Blobs.h"
#include "Blobs2.h"

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

#define NUM_SCENES 8
#define SCENE_BONDTRO 0
#define SCENE_RIBBONS1 1
#define SCENE_GENCINEMA 2
#define SCENE_SHAFTS 3
#define SCENE_BLOBS 4 
#define SCENE_BLOBS2 5
#define SCENE_RIBBONS2 6
#define SCENE_POMPOM 7
// #define SCENE_KNOTS

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

	// Instantiate all global Rocket tracks.
	CreateGlobalRocketTracks();

#if !defined(SYNC_PLAYER)
	// We're in edit mode: connect to Rocket client.
	if (0 != sync_connect(s_Rocket, rocketClient, SYNC_DEFAULT_PORT))
	{
		SetLastError("Unable to connect to a GNU Rocket client.");
		return false;
	}
#endif

	// We're now loading, but no disk I/O or anything else has taken place yet so it's an empty bar.
	DrawLoadProgress(nullptr, 0.f);

	// Set asset loader root path.
	Assets::SetRoot(assetsPath);

	// Create scenes.
	s_scenes.resize(NUM_SCENES);
	s_scenes[SCENE_BONDTRO] = new Bondtro();
	s_scenes[SCENE_RIBBONS1] = new Ribbons();
	s_scenes[SCENE_GENCINEMA] = new GeneralCinema();
	s_scenes[SCENE_SHAFTS] = new Shafts();
	s_scenes[SCENE_BLOBS] = new Blobs();
	s_scenes[SCENE_BLOBS2] = new Blobs2();
	s_scenes[SCENE_RIBBONS2] = new Ribbons2();
	s_scenes[SCENE_POMPOM] = new Pompom();

	// Instantiate all local (part/scene) Rocket tracks.	
	for (Scene *pScene : s_scenes)
		pScene->ReqRocketTracks();

	// Request resources.
	//

	texWhite = Pimp::gD3D->GetWhiteTex();

	for (Scene *pScene : s_scenes)
		pScene->ReqAssets();

	// Loaded some more! :)
	DrawLoadProgress(nullptr, 0.25f);

	// Req. global stuff.
	Assets::AddMaterial("shaders\\UserPosteffect.fx", &matUserPostFX);

	// Let the asset loader do it's thing up to where all disk I/O is verified.
	if (false == Assets::StartLoading())
		return false;

	// Loaded some more! :)
	DrawLoadProgress(nullptr, 0.5f);

	// Bind animation related nodes (and do other CPU-based preparation work).
	//
	
	// Add default camera transformation.
	s_defaultCam = new Pimp::Camera(gWorld);
	gWorld->GetElements().Add(s_defaultCam);
	s_defaultXform = new Pimp::Xform(gWorld);
	gWorld->GetElements().Add(s_defaultXform);
	AddChildToParent(s_defaultXform, gWorld->GetRootNode());
	AddChildToParent(s_defaultCam, s_defaultXform);
	s_defaultCam->SetFOVy(0.563197f);
	s_defaultXform->SetTranslation(Vector3(0.f, 0.f, 4.f));

	// Sprite batcher.
	s_sprites = new Pimp::Sprites();

	// Metaballs.
	s_pMetaballs = new Pimp::Metaballs();
	if (false == s_pMetaballs->Initialize())
		return false;

	// We're using Rocket- or debug-driven cameras only!
	gWorld->SetCurrentUserCamera(s_defaultCam);
	gWorld->SetUseCameraDirection(false);

	// Loaded some more! :)
	DrawLoadProgress(nullptr, 0.75f);

	// Wait for the asset loading to be all finished.
	if (false == Assets::FinishLoading())
		return false;

	// Setup and bind world objects.
	//

	for (Scene *pScene : s_scenes)
		pScene->BindToWorld();

	// Add user postFX.
	gWorld->GetMaterials().Add(matUserPostFX);
	gWorld->GetPostProcess()->SetUserPostEffect(matUserPostFX);

	// Ta-daa!
	DrawLoadProgress(nullptr, 1.f);
	
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
	delete s_pMetaballs;

	for (Scene *pScene : s_scenes)
		delete pScene;

	Assets::Release();

	if (nullptr != s_Rocket)
	{
		// Dump tracks to disk.
#ifndef SYNC_PLAYER
		sync_save_tracks(s_Rocket);
#endif

		sync_destroy_device(s_Rocket);
	}

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
	for (SyncTrack &syncTrack : s_syncTracks)
		syncTrack.Update(rocketRow);

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
	if (-1 != sceneIdx)
		s_scenes[sceneIdx]->Tick(rocketRow);
	else
		return false; // Demo is finished.

	// Tie in final flash and fade in the sprite batch at "improbable" Zs.
	const float postFlash = (float) sync_get_val(st_postFlash, rocketRow);
	const float postFade = (float) sync_get_val(st_postFade, rocketRow);
	const float kPostFlashZ = 5000.f;
	const float kPostFadeZ = 6000.f;
	if (postFlash != 0.f)
		s_sprites->AddSprite(
			texWhite,
			Pimp::D3D::BM_AlphaBlend,
			AlphaToVtxColor(postFlash, 0xffffff), 
			Vector2(0.f, 0.f), Vector2(1920.f, 1080.f),
			kPostFlashZ,
			0.f);
	if (postFade != 0.f)
		s_sprites->AddSprite(
			texWhite,
			Pimp::D3D::BM_AlphaBlend,
			AlphaToVtxColor(postFade, 0),
			Vector2(0.f, 0.f), Vector2(1920.f, 1080.f),
			kPostFadeZ,
			0.f);

	// This is primarily used to feed the debug camera if need be.
	if (nullptr != camOverride)
	{
		gWorld->SetCurrentUserCamera(camOverride);
	}

	return true;
}

void WorldRender() 
{
	// FIXME: hack, only pass metaballs object in scene's they're used to get drawn.
	const double rocketRow = Rocket_GetRow();
	const int sceneIdx = (int) sync_get_val(st_SceneIdx, rocketRow);
	gWorld->Render(s_sprites, ((SCENE_BLOBS == sceneIdx)||(SCENE_BLOBS2 == sceneIdx)) ? s_pMetaballs : nullptr); 
}


} // namespace Demo
