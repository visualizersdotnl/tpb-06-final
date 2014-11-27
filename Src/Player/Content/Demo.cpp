
#include <Core/Platform.h>
#include <Shared/fileutils.h>
#include <Shared/stopwatch.h>
#include <rocket/lib/sync.h>
#include <Core/Core.h>
#include "SceneTools.h"
#include "Assets.h"
#include "Audio.h"
#include "Settings.h"
#include "SetLastError.h"

namespace Demo {

// There is a single Core::World instance, which is a basic scenegraph.
static Pimp::World *s_pWorld = nullptr;
Pimp::World *GetWorld() { return s_pWorld; }

#include "Misc.h"
#include "Rocket.h"
#include "Scene.h"

//
// Global sync. tracks.
//

static const sync_track *st_SceneIdx;
static const sync_track *st_fxTime;
static const sync_track *st_postFlash ,*st_postFade;
static const sync_track *st_sceneFadeInOut;
static const sync_track *st_sceneNoise, *st_sceneNoiseT;
static const sync_track *st_defRotX, *st_defRotY, *st_defRotZ, *st_defRotW;
static const sync_track *st_defTransX, *st_defTransY, *st_defTransZ;
static const sync_track *st_noiseU, *st_noiseV, *st_noiseAlpha;

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

	s_syncTracks.push_back(SyncTrack("g_postNoiseU", false, &st_noiseU));
	s_syncTracks.push_back(SyncTrack("g_postNoiseV", false, &st_noiseV));
	s_syncTracks.push_back(SyncTrack("g_posNoiseAlpha", false, &st_noiseAlpha)); // FIXME: Typo!
}

 //
 // Shared objects.
 //

 // Default camera (animated using Rocket tracks).
static Pimp::Camera *s_defaultCam;
static Pimp::Xform *s_defaultXform;

// Textures.
static Pimp::Texture2D *texWhite;
static Pimp::Texture2D *texNoise;

// 2D sprite batcher.
static Pimp::Sprites *s_sprites = nullptr;

// Global material(s).
static Pimp::Material *matUserPostFX;

// Single metaballs instance (see .cpp for explanation).
static Pimp::Metaballs *s_pMetaballs = nullptr;
 
//
// Scenes.
//

#define NUM_SCENES 9
#define SCENE_BONDTRO 0
#define SCENE_RIBBONS1 1
#define SCENE_GENCINEMA 2
#define SCENE_SHAFTS 3
#define SCENE_BLOBS 4 
#define SCENE_BLOBS2 5
#define SCENE_RIBBONS2 6
#define SCENE_POMPOM 7
#define SCENE_BULLESANDBITCHES 8
// #define SCENE_KNOT

#include "Scenes/Bondtro.h"
#include "Scenes/Ribbons.h"
// #include "Scenes/Knot.h"
#include "Scenes/GeneralCinema.h"
#include "Scenes/Shafts.h"
#include "Scenes/Ribbons2.h"
#include "Scenes/Pompom.h"
#include "Scenes/BulletsAndBitches.h"

// Shared statics for blob parts (FIXME).
static const unsigned int kNumMetaball4s = 14;
static __declspec(align(16)) Pimp::Metaballs::Metaball4 s_metaball4s[kNumMetaball4s];
static const sync_track *st_blobsShininess, *st_blobsOverbright;
static const sync_track *st_blobsProjScrollU, *st_blobsProjScrollV;

#include "Scenes/Blobs.h"
#include "Scenes/Blobs2.h"

// Asset root directory.
const std::string GetAssetsPath()
{
	std::string exePath = RemoveFilenameFromPath(GetCurrentProcessFileName());
	std::transform(exePath.begin(), exePath.end(), exePath.begin(), ::tolower);
	return exePath + "assets\\";
}

// World initialization.
bool GenerateWorld(const char *rocketClient)
{
	s_pWorld = new Pimp::World();

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
	DrawLoadProgress(*s_pWorld, 0.f);

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
	s_scenes[SCENE_BULLESANDBITCHES] = new BulletsAndBitches();

	// Instantiate all local (part/scene) Rocket tracks.	
	for (auto pScene : s_scenes)
		pScene->ReqRocketTracks();

	// 2 blob parts share these (must be uploaded manually since it's not a dyn. loaded shader).
	s_syncTracks.push_back(SyncTrack("blobsShininess", false, &st_blobsShininess));
	s_syncTracks.push_back(SyncTrack("blobsOverbright", false, &st_blobsOverbright));
	s_syncTracks.push_back(SyncTrack("blobsProjScrollU", false, &st_blobsProjScrollU));
	s_syncTracks.push_back(SyncTrack("blobsProjScrollV", false, &st_blobsProjScrollV));

	// Request resources.
	//

	texWhite = Pimp::gD3D->GetWhiteTex();

	for (auto pScene : s_scenes)
		pScene->ReqAssets();

	// Loaded some more! :)
	DrawLoadProgress(*s_pWorld, 0.25f);

	// Req. global stuff.
	Assets::AddMaterial("shaders\\UserPosteffect.fx", &matUserPostFX);
	Assets::AddTexture2D("textures\\noise.png", &texNoise);

	// Let the asset loader do it's thing up to where all disk I/O is verified.
	if (false == Assets::StartLoading())
		return false;

	// Loaded some more! :)
	DrawLoadProgress(*s_pWorld, 0.5f);

	// Bind animation related nodes (and do other CPU-based preparation work).
	//
	
	// Add default camera transformation.
	s_defaultCam = new Pimp::Camera(s_pWorld);
	s_pWorld->GetElements().push_back(s_defaultCam);
	s_defaultXform = new Pimp::Xform(s_pWorld);
	s_pWorld->GetElements().push_back(s_defaultXform);
	AddChildToParent(s_defaultXform, s_pWorld->GetRootNode());
	AddChildToParent(s_defaultCam, s_defaultXform);
	s_defaultCam->SetFOVy(0.563197f);
	s_defaultXform->SetTranslation(Vector3(0.f, 0.f, 4.f));

	// Sprite batcher.
	s_sprites = new Pimp::Sprites(PIMPPLAYER_SPRITE_RES_X, PIMPPLAYER_SPRITE_RES_Y);

	// Metaballs.
	s_pMetaballs = new Pimp::Metaballs();
	if (false == s_pMetaballs->Initialize())
		return false;

	// We're using Rocket- or debug-driven cameras only!
	s_pWorld->SetCamera(s_defaultCam);

	// Loaded some more! :)
	DrawLoadProgress(*s_pWorld, 0.75f);

	// Wait for the asset loading to be all finished.
	if (false == Assets::FinishLoading())
		return false;

	// Setup and bind world objects.
	//

	for (auto pScene : s_scenes)
		pScene->BindToWorld();

	// Add user postFX.
	s_pWorld->GetMaterials().push_back(matUserPostFX);
	s_pWorld->GetPostProcess().SetUserPostEffect(matUserPostFX);

	// Ta-daa, ready.
	DrawLoadProgress(*s_pWorld, 1.f);

#if !defined(_DEBUG) && !defined(_DESIGN)
	// Delibrate delay. Make it appear as if we're doing important things.
	if (true == PIMPPLAYER_RUN_FROM_SHADER_BINARIES)
		Sleep(1500);
#endif
	
	// Finish up some World business.
	s_pWorld->UpdateAllMaterialParameters();

#if defined(SYNC_PLAYER)
	// We're in replay mode so start the soundtrack, otherwise Rocket will tell us what to do.
	Audio_Start();
#endif

	// Switch off loading bar in shader.
	// No frames will (or should) be rendered before the actual demo starts.
	s_pWorld->GetPostProcess().SetLoadProgress(0.f);

	// Done!
	return true;
}

// Releases all resources.
void ReleaseWorld()
{
	delete s_sprites;
	delete s_pMetaballs;

	for (auto pScene : s_scenes)
		delete pScene;

	Assets::Release();

	if (nullptr != s_Rocket)
	{
		// Dump tracks to disk.
		// This way they're pretty much always ready to use.
#ifndef SYNC_PLAYER
		sync_save_tracks(s_Rocket);
#endif

		sync_destroy_device(s_Rocket);
	}

	delete s_pWorld;
	s_pWorld = nullptr;
}

// Tick function. 
// Manipulate the world and it's objects according to sync. prior to rendering it.
bool Tick(float timeElapsed, Pimp::Camera *pDebugCam)
{
	// Get sync. row.
	double rocketRow = Rocket_GetRow();

#if !defined(SYNC_PLAYER)
	if (0 != sync_update(s_Rocket, int(rocketRow), &s_rocketHooks, nullptr))
	{
		SetLastError("Connection to GNU Rocket lost!");
		return false;
	}
#else
	// Taken from Kusma's engine. Seems like an arbitrary delay bias, he couldn't really explain.
//	rocketRow += 0.005f; 
#endif

	// Update sync. tracks.
	for (SyncTrack &syncTrack : s_syncTracks)
		syncTrack.Update(rocketRow);

	// Update default camera.
	const float defCamTrans_X   = (float) sync_get_val(st_defTransX, rocketRow);
	const float defCamTrans_Y   = (float) sync_get_val(st_defTransY, rocketRow);
	const float defCamTrans_Z   = (float) sync_get_val(st_defTransZ, rocketRow);
	const float defCamRotQuat_X = (float) sync_get_val(st_defRotX,   rocketRow);
	const float defCamRotQuat_Y = (float) sync_get_val(st_defRotY,   rocketRow);
	const float defCamRotQuat_Z = (float) sync_get_val(st_defRotZ,   rocketRow);
	const float defCamRotQuat_W = (float) sync_get_val(st_defRotW,   rocketRow);
	s_defaultXform->SetTranslation(Vector3(defCamTrans_X, defCamTrans_Y, defCamTrans_Z));
	s_defaultXform->SetRotation(Quaternion(defCamRotQuat_X, defCamRotQuat_Y, defCamRotQuat_Z, defCamRotQuat_W));

	// No flanger by default (TPB-06).
	Audio_FlangerMP3(0.f, 0.25f);

	const int sceneIdx = (int) sync_get_val(st_SceneIdx, rocketRow);
	if (-1 != sceneIdx)
		s_scenes[sceneIdx]->Tick(rocketRow);
	else
	{
		// FIXME: A regular demo would stop right here, TPB-06 has a little Bond hack on top.
//		return false;

		// ----- TPB-06 ENDING HACK -----

		Audio_Pause();

		static Stopwatch stopwatch;
		static bool resetTimer = true;
		if (true == resetTimer)
		{
			stopwatch.Reset();
			resetTimer = false;
		}

		const float kEndTime = 12.f;
		const float time = stopwatch.GetSecondsElapsed();

		float alpha = 1.f;
		if (time < 2.f) alpha = 0.f;
		if (time > 2.f && time < 4.f) alpha = (time-2.f)/2.f;
		else if (time > kEndTime-2.f) alpha = 1.f - ( (time-(kEndTime-2.f)) / 2.f );

		static bool shellDropped = false;
		if (false == shellDropped && time > 2.f)
		{
			Audio_Shelldrop();
			shellDropped = true;
		}

		((BulletsAndBitches*)s_scenes[SCENE_BULLESANDBITCHES])->EndPic(alpha);

		// Wait for esc. or kEndTime.
		return time < kEndTime;

		// ----- TPB-06 ENDING HACK -----
	}

	// Sprite post FX Zs.
	const float kPostNoiseZ = 10000.f;
	const float kPostFlashZ = 20000.f;
	const float kPostFadeZ  = 30000.f;

	// Tie in noise the old school way by random displacing a texture.
	const float noiseAlpha = (float) sync_get_val(st_noiseAlpha, rocketRow);
	if (noiseAlpha != 0.f)
	{
		float noiseU = (float) sync_get_val(st_noiseU, rocketRow);
		float noiseV = (float) sync_get_val(st_noiseV, rocketRow);
		noiseU += randf((float)texNoise->GetWidth());
		noiseV += randf((float)texNoise->GetHeight());
		s_sprites->AddSprite(
			texNoise,
			Pimp::D3D::BM_Additive,
			AlphaToVtxColor(noiseAlpha, 0xffffff), 
			Vector2(0.f, 0.f), Vector2(1920.f, 1080.f),
			kPostNoiseZ,
			0.f,   // rotZ
			false, // forceClamp
			false, // isBackground
			Vector2(2.f*(PIMPPLAYER_RENDER_ASPECT_RATIO), 2.f),
			Vector2(noiseU, noiseV));
	}

	// Tie in final flash and fade in.
	const float postFlash = (float) sync_get_val(st_postFlash, rocketRow);
	const float postFade = (float) sync_get_val(st_postFade, rocketRow);
	if (postFlash != 0.f)
		s_sprites->AddSprite(
			texWhite,
			Pimp::D3D::BM_AlphaBlend,
			AlphaToVtxColor(postFlash, 0xffffff), 
			Vector2(0.f, 0.f), Vector2(1920.f, 1080.f),
			kPostFlashZ, 0.f, true, false);
	if (postFade != 0.f)
		s_sprites->AddSprite(
			texWhite,
			Pimp::D3D::BM_AlphaBlend,
			AlphaToVtxColor(postFade, 0),
			Vector2(0.f, 0.f), Vector2(1920.f, 1080.f),
			kPostFadeZ, 0.f, true, false);

	// Debug camera?
	if (nullptr != pDebugCam)
	{
		s_pWorld->SetCamera(pDebugCam);
		timeElapsed = 0.f; // Freeze world (apart from Rocket timing, obviously).
	}

	// Update world elements.
	s_pWorld->Tick(timeElapsed);

	return true;
}

void WorldRender() 
{
	const double rocketRow = Rocket_GetRow();
	const int sceneIdx = (int) sync_get_val(st_SceneIdx, rocketRow);

	// FIXME: hack, only pass metaballs object in scenes they're used.
	s_pWorld->Render(*s_sprites, ((SCENE_BLOBS == sceneIdx)||(SCENE_BLOBS2 == sceneIdx)) ? s_pMetaballs : nullptr); 
}


} // namespace Demo
