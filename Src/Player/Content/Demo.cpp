
#include <Core/Core.h>
#include <Shared/shared.h>
#include <LodePNG/lodepng.h>
#include "SceneTools.h"
#include "Assets.h"

namespace Demo {

// Assets root.
//

static const std::string GetAssetsPath()
{
	// FIXME: Append /Content here and move stuff.
	std::string path = LowerCase(RemoveFilenameFromPath(GetCurrentProcessFileName()));
	return path;
}

// World generator & resource release.
//

bool GenerateWorld(Pimp::World** outWorld)
{
	// Store new world pointer right away; DrawLoadProgress assumes there is a gWorld.
	Pimp::World* world = new Pimp::World();
	*outWorld = world; 

	// We're now loading, but no disk I/O or anything else has taken place yet so it's an empty bar.
	DrawLoadProgress(false);

	// Set asset loader root.
	Assets::SetRoot(GetAssetsPath());
	const std::string assetsPath = GetAssetsPath();

	// Asset requests for all scenes.
	//

	// Test textures.
	Pimp::Texture2D *blurbGrid, *blurbNoise, *blurbRock, *testOverlay;
	Pimp::Texture2D *ribbonsWall, *ribbonsMesh;
	Assets::AddTexture2D("blurb_grid.png", true, &blurbGrid);
	Assets::AddTexture2D("blurb_noise.png", true, &blurbNoise);
	Assets::AddTexture2D("blurb_rock.png", true, &blurbRock);
	Assets::AddTexture2D("testoverlay.png", true, &testOverlay);
	Assets::AddTexture2D("ribbons_wall.png", true, &ribbonsWall);
	Assets::AddTexture2D("ribbons_mesh.png", true, &ribbonsMesh);

	// ----------------------------------------------------------------------------------------------

	// Set total number of material compilation jobs. Assures that the loading bar is accurate.
	int numScenes = 2;
	int numOverlays = 1;
	SetNumTotalMaterialCompilationJobs(numScenes + numOverlays + 1);

	// Scene shader 0
	unsigned char* sceneShader0_hlsl;
	int sceneShader0_hlsl_size;
	ReadFileContents(assetsPath + "scene_blurb.fx", &sceneShader0_hlsl, &sceneShader0_hlsl_size);

	unsigned char* sceneShaderMat0_compiled_hlsl = NULL;
	int sceneShaderMat0_compiled_hlsl_size = 0;
	StartMaterialCompilationJob(sceneShader0_hlsl, sceneShader0_hlsl_size, &sceneShaderMat0_compiled_hlsl, &sceneShaderMat0_compiled_hlsl_size);

	// Scene shader 1
	unsigned char* sceneShader1_hlsl;
	int sceneShader1_hlsl_size;
	ReadFileContents(assetsPath + "scene_ribbons.fx", &sceneShader1_hlsl, &sceneShader1_hlsl_size);

	unsigned char* sceneShaderMat1_compiled_hlsl = NULL;
	int sceneShaderMat1_compiled_hlsl_size = 0;
	StartMaterialCompilationJob(sceneShader1_hlsl, sceneShader1_hlsl_size, &sceneShaderMat1_compiled_hlsl, &sceneShaderMat1_compiled_hlsl_size);


	// Post effect
	unsigned char* userPostEffect_hlsl;
	int userPostEffect_hlsl_size;
	ReadFileContents(assetsPath + "posteffect.fx", &userPostEffect_hlsl, &userPostEffect_hlsl_size);

	unsigned char* userPostEffectMat_compiled_hlsl = NULL;
	int userPostEffectMat_compiled_hlsl_size = 0;
	StartMaterialCompilationJob(userPostEffect_hlsl, userPostEffect_hlsl_size, &userPostEffectMat_compiled_hlsl, &userPostEffectMat_compiled_hlsl_size);

	// Overlay shader 0
	unsigned char* overlayShader0_hlsl;
	int overlayShader0_hlsl_size;
	ReadFileContents(assetsPath + "overlay_test.fx", &overlayShader0_hlsl, &overlayShader0_hlsl_size);

	unsigned char* overlayShaderMat0_compiled_hlsl = NULL;
	int overlayShaderMat0_compiled_hlsl_size = 0;
	StartMaterialCompilationJob(overlayShader0_hlsl, overlayShader0_hlsl_size, &overlayShaderMat0_compiled_hlsl, &overlayShaderMat0_compiled_hlsl_size);

	// ----------------------------------------------------------------------------------------------

	// Let the asset loader do it's thing up to where all disk I/O is completed and verified.
	//

	// Doing this first as it possibly kicks off a lot of threads.
	if (false == Assets::LoadMaterials())
		return false;

	if (false == Assets::LoadTextures())
		return false;

	// And by now all I/O tasks are done, so set the loading bar accordingly.
	DrawLoadProgress(true);

	// ----------------------------------------------------------------------------------------------

	// Add camera
	Pimp::Camera* camTestShape = new Pimp::Camera(world);
	world->GetElements().Add(camTestShape);
	camTestShape->SetFOVy(0.563197f);

	Pimp::Xform* xformCamTest = new Pimp::Xform(world);
	world->GetElements().Add(xformCamTest);

	// Add material params
	Pimp::MaterialParameter* paramSpherePos0 = new Pimp::MaterialParameter(world);
	Pimp::MaterialParameter* paramSpherePos1 = new Pimp::MaterialParameter(world);
	Pimp::MaterialParameter* paramSpherePos2 = new Pimp::MaterialParameter(world);
	world->GetElements().Add(paramSpherePos0);
	world->GetElements().Add(paramSpherePos1);
	world->GetElements().Add(paramSpherePos2);
	paramSpherePos0->SetValueType(Pimp::MaterialParameter::VT_NodeXformInv);
	paramSpherePos1->SetValueType(Pimp::MaterialParameter::VT_NodeXformInv);
	paramSpherePos2->SetValueType(Pimp::MaterialParameter::VT_NodeXformInv);
	paramSpherePos0->SetName("testBallXformInv0");
	paramSpherePos1->SetName("testBallXformInv1");
	paramSpherePos2->SetName("testBallXformInv2");

	Pimp::Xform* xformParamSpherePos0 = new Pimp::Xform(world);
	Pimp::Xform* xformParamSpherePos1 = new Pimp::Xform(world);
	Pimp::Xform* xformParamSpherePos2 = new Pimp::Xform(world);
	world->GetElements().Add(xformParamSpherePos0);
	world->GetElements().Add(xformParamSpherePos1);
	world->GetElements().Add(xformParamSpherePos2);

	Pimp::MaterialParameter* paramSceneTime = new Pimp::MaterialParameter(world);
	world->GetElements().Add(paramSceneTime);
	paramSceneTime->SetValueType(Pimp::MaterialParameter::VT_Value);
	paramSceneTime->SetName("testSceneTime");
	
	// Set up anim curves for camera xform
	Pimp::AnimCurve* camTestShape_posX = new Pimp::AnimCurve(world);
	Pimp::AnimCurve* camTestShape_posY = new Pimp::AnimCurve(world);
	Pimp::AnimCurve* camTestShape_posZ = new Pimp::AnimCurve(world);
	Pimp::AnimCurve* camTestShape_rotX = new Pimp::AnimCurve(world);
	Pimp::AnimCurve* camTestShape_rotY = new Pimp::AnimCurve(world);
	Pimp::AnimCurve* camTestShape_rotZ = new Pimp::AnimCurve(world);
	world->GetElements().Add(camTestShape_posX);
	world->GetElements().Add(camTestShape_posY);
	world->GetElements().Add(camTestShape_posZ);
	world->GetElements().Add(camTestShape_rotX);
	world->GetElements().Add(camTestShape_rotY);
	world->GetElements().Add(camTestShape_rotZ);
	static const Pimp::AnimCurve::Pair camTestShape_posX_keys[] = { 
		{  0.0f,	-0.36f, 0.0f, 0.0f}, // pos.x
		{  20.0f,	0.38f, 0.0f, 0.0f}, // pos.x
	};
	static const Pimp::AnimCurve::Pair camTestShape_posY_keys[] = { 
		{  0.0f,	6.26f, 0.0f, 0.0f}, // pos.y
		{  20.0f,	12.20f, 0.0f, 0.0f}, // pos.y
	};
	static const Pimp::AnimCurve::Pair camTestShape_posZ_keys[] = { 
		{  0.0f,	7.35f, 0.0f, 0.0f}, // pos.z
		{  20.0f,	7.76f, 0.0f, 0.0f}, // pos.z
	};
	static const Pimp::AnimCurve::Pair camTestShape_rotX_keys[] = { 
		{  0.0f,	DEG2RAD(-0.90f), 0.0f, 0.0f}, // rot.x
		{  20.0f,	DEG2RAD(5.38f), 0.0f, 0.0f}, // rot.x
	};
	static const Pimp::AnimCurve::Pair camTestShape_rotY_keys[] = { 
		{  0.0f,	DEG2RAD(-7.99f), 0.0f, 0.0f}, // rot.y
		{  20.0f,	DEG2RAD(-10.28f), 0.0f, 0.0f}, // rot.y
	};
	static const Pimp::AnimCurve::Pair camTestShape_rotZ_keys[] = { 
		{  0.0f,	DEG2RAD(82.94f), 0.0f, 0.0f}, // rot.z
		{  20.0f,	DEG2RAD(83.10f), 0.0f, 0.0f}, // rot.z
	};
	camTestShape_posX->SetKeysPtr(camTestShape_posX_keys, sizeof(camTestShape_posX_keys)/sizeof(Pimp::AnimCurve::Pair));
	camTestShape_posY->SetKeysPtr(camTestShape_posY_keys, sizeof(camTestShape_posY_keys)/sizeof(Pimp::AnimCurve::Pair));
	camTestShape_posZ->SetKeysPtr(camTestShape_posZ_keys, sizeof(camTestShape_posZ_keys)/sizeof(Pimp::AnimCurve::Pair));
	camTestShape_rotX->SetKeysPtr(camTestShape_rotX_keys, sizeof(camTestShape_rotX_keys)/sizeof(Pimp::AnimCurve::Pair));
	camTestShape_rotY->SetKeysPtr(camTestShape_rotY_keys, sizeof(camTestShape_rotY_keys)/sizeof(Pimp::AnimCurve::Pair));
	camTestShape_rotZ->SetKeysPtr(camTestShape_rotZ_keys, sizeof(camTestShape_rotZ_keys)/sizeof(Pimp::AnimCurve::Pair));

	xformCamTest->SetAnimCurveTranslationX(camTestShape_posX);
	xformCamTest->SetAnimCurveTranslationY(camTestShape_posY);
	xformCamTest->SetAnimCurveTranslationZ(camTestShape_posZ);
	xformCamTest->SetAnimCurveRotationX(camTestShape_rotX);
	xformCamTest->SetAnimCurveRotationY(camTestShape_rotY);
	xformCamTest->SetAnimCurveRotationZ(camTestShape_rotZ);


	// Set up anim curves for material param xform
	Pimp::AnimCurve* paramSpherePos_posX = new Pimp::AnimCurve(world);
	Pimp::AnimCurve* paramSpherePos_posY = new Pimp::AnimCurve(world);
	Pimp::AnimCurve* paramSpherePos_posZ = new Pimp::AnimCurve(world);
	Pimp::AnimCurve* paramSpherePos_rotX = new Pimp::AnimCurve(world);
	Pimp::AnimCurve* paramSpherePos_rotY = new Pimp::AnimCurve(world);
	Pimp::AnimCurve* paramSpherePos_rotZ = new Pimp::AnimCurve(world);
	world->GetElements().Add(paramSpherePos_posX);
	world->GetElements().Add(paramSpherePos_posY);
	world->GetElements().Add(paramSpherePos_posZ);
	world->GetElements().Add(paramSpherePos_rotX);
	world->GetElements().Add(paramSpherePos_rotY);
	world->GetElements().Add(paramSpherePos_rotZ);
	static const Pimp::AnimCurve::Pair paramSpherePos_posX_keys[] = { 
		{ 0.0000000f,	0.0f, 0.0f, 0.0f},
	};
	static const Pimp::AnimCurve::Pair paramSpherePos_posY_keys[] = { 
		{  0.0f,	-2.0f, 0.0f, 0.0f},
		{  2.0f,	+2.0f, 0.0f, 0.0f},
		{  4.0f,	-2.0f, 0.0f, 0.0f},
		{  6.0f,	+2.0f, 0.0f, 0.0f},
		{  8.0f,	-2.0f, 0.0f, 0.0f},
		{ 10.0f,	+2.0f, 0.0f, 0.0f},
		{ 12.0f,	-2.0f, 0.0f, 0.0f},
		{ 14.0f,	+2.0f, 0.0f, 0.0f},
		{ 16.0f,	-2.0f, 0.0f, 0.0f},
		{ 18.0f,	+2.0f, 0.0f, 0.0f},
	};
	static const Pimp::AnimCurve::Pair paramSpherePos_posZ_keys[] = { 
		{ 0.0000000f,	-3.3f, 0.0f, 0.0f},
	};
	static const Pimp::AnimCurve::Pair paramSpherePos_rotX_keys[] = { 
		{  0.0f,	DEG2RAD(0.0f), 0.0f, 0.0f}, // rot.x
		{ 18.0f,	DEG2RAD(1134.25f), 0.0f, 0.0f}, // rot.x
	};
	static const Pimp::AnimCurve::Pair paramSpherePos_rotY_keys[] = { 
		{  0.0f,	DEG2RAD(0.0f), 0.0f, 0.0f}, // rot.y
		{ 18.0f,	DEG2RAD(145.81f), 0.0f, 0.0f}, // rot.y
	};
	static const Pimp::AnimCurve::Pair paramSpherePos_rotZ_keys[] = { 
		{  0.0f,	DEG2RAD(0.0f), 0.0f, 0.0f}, // rot.z
		{ 18.0f,	DEG2RAD(1172.53f), 0.0f, 0.0f}, // rot.z
	};
	paramSpherePos_posX->SetKeysPtr(paramSpherePos_posX_keys, sizeof(paramSpherePos_posX_keys)/sizeof(Pimp::AnimCurve::Pair));
	paramSpherePos_posY->SetKeysPtr(paramSpherePos_posY_keys, sizeof(paramSpherePos_posY_keys)/sizeof(Pimp::AnimCurve::Pair));
	paramSpherePos_posZ->SetKeysPtr(paramSpherePos_posZ_keys, sizeof(paramSpherePos_posZ_keys)/sizeof(Pimp::AnimCurve::Pair));
	paramSpherePos_rotX->SetKeysPtr(paramSpherePos_rotX_keys, sizeof(paramSpherePos_rotX_keys)/sizeof(Pimp::AnimCurve::Pair));
	paramSpherePos_rotY->SetKeysPtr(paramSpherePos_rotY_keys, sizeof(paramSpherePos_rotY_keys)/sizeof(Pimp::AnimCurve::Pair));
	paramSpherePos_rotZ->SetKeysPtr(paramSpherePos_rotZ_keys, sizeof(paramSpherePos_rotZ_keys)/sizeof(Pimp::AnimCurve::Pair));

	xformParamSpherePos0->SetAnimCurveTranslationX(paramSpherePos_posX);
	xformParamSpherePos0->SetAnimCurveTranslationY(paramSpherePos_posY);
	xformParamSpherePos0->SetAnimCurveTranslationZ(paramSpherePos_posZ);
	xformParamSpherePos0->SetAnimCurveRotationX(paramSpherePos_rotX);
	xformParamSpherePos0->SetAnimCurveRotationY(paramSpherePos_rotY);
	xformParamSpherePos0->SetAnimCurveRotationZ(paramSpherePos_rotZ);

	float delay = 0.3f;
	xformParamSpherePos1->SetAnimCurveTranslationX(paramSpherePos_posX);
	xformParamSpherePos1->SetAnimCurveTranslationY(paramSpherePos_posY);
	xformParamSpherePos1->SetAnimCurveTranslationZ(paramSpherePos_posZ);
	xformParamSpherePos1->SetAnimCurveRotationX(DuplicateAnimCurve(world, paramSpherePos_rotX, delay));
	xformParamSpherePos1->SetAnimCurveRotationY(DuplicateAnimCurve(world, paramSpherePos_rotY, delay));
	xformParamSpherePos1->SetAnimCurveRotationZ(DuplicateAnimCurve(world, paramSpherePos_rotZ, delay));
	
	delay *= 2.0f;
	xformParamSpherePos2->SetAnimCurveTranslationX(paramSpherePos_posX);
	xformParamSpherePos2->SetAnimCurveTranslationY(paramSpherePos_posY);
	xformParamSpherePos2->SetAnimCurveTranslationZ(paramSpherePos_posZ);
	xformParamSpherePos2->SetAnimCurveRotationX(DuplicateAnimCurve(world, paramSpherePos_rotX, delay));
	xformParamSpherePos2->SetAnimCurveRotationY(DuplicateAnimCurve(world, paramSpherePos_rotY, delay));
	xformParamSpherePos2->SetAnimCurveRotationZ(DuplicateAnimCurve(world, paramSpherePos_rotZ, delay));

	// Set up anim curve for material param scene time
	Pimp::AnimCurve* paramSceneTime_value = new Pimp::AnimCurve(world);
	world->GetElements().Add(paramSceneTime_value);
	static const Pimp::AnimCurve::Pair paramSceneTime_value_keys[] = { 
		{ 0.0000000f,	0.0f, 0.0f, 0.0f},
		{ 100.0000f,	100.0f, 0.0f, 0.0f},
	};
	paramSceneTime_value->SetKeysPtr(paramSceneTime_value_keys, sizeof(paramSceneTime_value_keys)/sizeof(Pimp::AnimCurve::Pair));
	paramSceneTime->SetAnimCurveValue(paramSceneTime_value);

	// Build up our scene graph

	AddChildToParent(xformCamTest,world->GetRootNode());
	AddChildToParent(xformParamSpherePos0,world->GetRootNode());
	AddChildToParent(xformParamSpherePos1,world->GetRootNode());
	AddChildToParent(xformParamSpherePos2,world->GetRootNode());
	AddChildToParent(paramSceneTime, world->GetRootNode());
	
	AddChildToParent(camTestShape,xformCamTest);
	AddChildToParent(paramSpherePos0,xformParamSpherePos0);
	AddChildToParent(paramSpherePos1,xformParamSpherePos1);
	AddChildToParent(paramSpherePos2,xformParamSpherePos2);


	// Camera direction (add all cameras)

	world->SetUseCameraDirection(true);
	FixedSizeList<Pimp::Camera*>& directionCameras = world->GetDirectionCameras();

	directionCameras.Add(camTestShape); // 0: test cam



	static const Pimp::AnimCurve::Pair cameraDirection_keys[] = { 
	 { -1.000000f,0.000000f,0.000000f,0.000000f},	// cam 0
	};
	world->GetCameraDirectionAnimCurve()->SetKeysPtr(cameraDirection_keys, sizeof(cameraDirection_keys)/sizeof(Pimp::AnimCurve::Pair));


	static const Pimp::AnimCurve::Pair sceneDirection_keys[] = { 
	 { -1.000000f, 1.000000f,0.000000f,0.000000f},	// scene 1, always active...
	};
	world->GetSceneDirectionAnimCurve()->SetKeysPtr(sceneDirection_keys, sizeof(sceneDirection_keys)/sizeof(Pimp::AnimCurve::Pair));

	// Wait for the asset loading thread(s) to be all finished.
	//

	Assets::FinishLoading();

	// Add test textures to our World.
	//

	world->GetTextures().Add(blurbGrid);
	world->GetTextures().Add(blurbNoise);
	world->GetTextures().Add(blurbRock);
	world->GetTextures().Add(testOverlay);
	world->GetTextures().Add(ribbonsWall);
	world->GetTextures().Add(ribbonsMesh);

	// ----------------------------------------------------------------------------------------------

	// Scenes
	Pimp::Material* sceneShaderMat0 = new Pimp::Material(world, sceneShaderMat0_compiled_hlsl, sceneShaderMat0_compiled_hlsl_size, assetsPath + "scene_blurb.fx");
	world->GetMaterials().Add(sceneShaderMat0);
	Pimp::Scene* sceneShader0 = new Pimp::Scene(world);
	sceneShader0->SetMaterial(sceneShaderMat0);
	world->GetScenes().Add(sceneShader0);
	world->GetElements().Add(sceneShader0);

	Pimp::Material* sceneShaderMat1 = new Pimp::Material(world, sceneShaderMat1_compiled_hlsl, sceneShaderMat1_compiled_hlsl_size, assetsPath + "scene_ribbons.fx");
	world->GetMaterials().Add(sceneShaderMat1);
	Pimp::Scene* sceneShader1 = new Pimp::Scene(world);
	sceneShader1->SetMaterial(sceneShaderMat1);
	world->GetScenes().Add(sceneShader1);
	world->GetElements().Add(sceneShader1);


	// Post effect
	Pimp::Material* userPostEffectMat = new Pimp::Material(world, userPostEffectMat_compiled_hlsl, userPostEffectMat_compiled_hlsl_size, assetsPath + "posteffect.fx");
	world->GetMaterials().Add(userPostEffectMat);
	world->GetPostProcess()->SetUserPostEffect(userPostEffectMat);

	// Overlays
	Pimp::Material* overlayShaderMat0 = new Pimp::Material(world, overlayShaderMat0_compiled_hlsl, overlayShaderMat0_compiled_hlsl_size, assetsPath + "overlay_test.fx");
	world->GetMaterials().Add(overlayShaderMat0);
	Pimp::Overlay* overlayShader0 = new Pimp::Overlay(world);
	overlayShader0->SetMaterial(overlayShaderMat0);
	overlayShader0->SetTimings(0.0f, 100.0f, 0.5f, 0.5f);
	world->GetOverlays().Add(overlayShader0);
	world->GetElements().Add(overlayShader0);	

	// ----------------------------------------------------------------------------------------------

	// Do stuff.
	//

	world->InitAllBalls();
	world->UpdateAllMaterialParameters();

	// Rock and roll baby!
	//

	*outWorld = world;
	return true;
}

void ReleaseWorld()
{
	Assets::Release();
	// FIXME: Release more resources?
}

// Tick function (manipulate world according to sync., prior to "ticking" & rendering it).
//

void Tick()
{
}

} // namespace Demo
