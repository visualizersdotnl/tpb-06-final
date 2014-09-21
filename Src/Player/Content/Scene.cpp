
#include <core/core.h>
#include <shared/shared.h>
#include "SceneTools.h"
#include "LodePNG/lodepng.h"


std::string GetAssetsPath()
{
	// FIXME: Append /Content here and move stuff.
	std::string path = LowerCase(RemoveFilenameFromPath(GetCurrentProcessFileName()));
	return path;
}

// Load a PNG texture from a file. 
// FIXME: Should be moved to Texture2D::LoadFromFile(), probably.
Pimp::Texture2D* LoadTexture(const std::string& filename, bool requiresGammaCorrection)
{
	std::string name = LowerCase(GetFilenameWithoutExtFromPath(filename));

	std::vector<unsigned char> pixels;
	unsigned int width, height;

	unsigned int error = lodepng::decode(pixels, width, height, filename);
	//lodepng_decode32_file(&pixels, &width, &height, filename.c_str());

	ASSERT_MSG(error == 0, std::string("Error loading texture from '") + filename + std::string("': ") + lodepng_error_text(error));
	ASSERT_MSG(width == height, std::string("Error loading texture from '") + filename + std::string("'. It's not square. Currently only square textures are supported..."));
	ASSERT(pixels.size() > 0);

	int sizePixels = width;

	Pimp::Texture2D* texture = Pimp::gD3D->CreateTexture2D(name, sizePixels, requiresGammaCorrection);

	texture->UploadTexels(&pixels[0]);

	return texture;
}

void GenerateWorld(Pimp::World** outWorld)
{
	Pimp::World* world = new Pimp::World();
	
	*outWorld = world; // Store world pointer right away; DrawLoadProgress assumes there is a gWorld.
	
	DrawLoadProgress(false);

	std::string assetsPath = GetAssetsPath();


	// ---------------------------------------------------------------------------------------------------------------------------------------------------
	// Shaders

	// Set total number of material compilation jobs. Assures that the loading bar is accurate.
	SetNumTotalMaterialCompilationJobs(2);

	// Scene shader 0
	Pimp::Scene* sceneShader0 = new Pimp::Scene(world);
	unsigned char* sceneShader0_hlsl;
	int sceneShader0_hlsl_size;
	ReadFileContents(assetsPath + "scene_test.fx", &sceneShader0_hlsl, &sceneShader0_hlsl_size);

	unsigned char* sceneShaderMat0_compiled_hlsl = NULL;
	int sceneShaderMat0_compiled_hlsl_size = 0;
	StartMaterialCompilationJob(sceneShader0_hlsl, sceneShader0_hlsl_size, &sceneShaderMat0_compiled_hlsl, &sceneShaderMat0_compiled_hlsl_size);

	// Post effect
	unsigned char* userPostEffect_hlsl;
	int userPostEffect_hlsl_size;
	ReadFileContents(assetsPath + "posteffect.fx", &userPostEffect_hlsl, &userPostEffect_hlsl_size);

	unsigned char* userPostEffectMat_compiled_hlsl = NULL;
	int userPostEffectMat_compiled_hlsl_size = 0;
	StartMaterialCompilationJob(userPostEffect_hlsl, userPostEffect_hlsl_size, &userPostEffectMat_compiled_hlsl, &userPostEffectMat_compiled_hlsl_size);


	// ---------------------------------------------------------------------------------------------------------------------------------------------------
	// Construct and initialize all elements.

	world->SetMotionBlurAmount(0.000000f);


	
	DrawLoadProgress(true);
	

	// Add camera
	Pimp::Camera* camTestShape = new Pimp::Camera(world);
	world->GetElements().Add(camTestShape);
	camTestShape->SetFOVy(0.563197f);

	Pimp::Xform* xformCamTest = new Pimp::Xform(world);
	world->GetElements().Add(xformCamTest);

	// Add material params
	Pimp::MaterialParameter* paramSpherePos = new Pimp::MaterialParameter(world);
	world->GetElements().Add(paramSpherePos);
	paramSpherePos->SetValueType(Pimp::MaterialParameter::VT_NodePosition);
	paramSpherePos->SetName("testBallPos");

	Pimp::Xform* xformParamSpherePos = new Pimp::Xform(world);
	world->GetElements().Add(xformParamSpherePos);

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
		{  0.0f,	-4.50f, 0.0f, 0.0f}, // pos.x
		{ 10.0f,	-6.76f, 0.0f, 0.0f}, // pos.x
	};
	static const Pimp::AnimCurve::Pair camTestShape_posY_keys[] = { 
		{  0.0f,	-19.91f, 0.0f, 0.0f}, // pos.y
		{ 10.0f,	-21.73f, 0.0f, 0.0f}, // pos.y
	};
	static const Pimp::AnimCurve::Pair camTestShape_posZ_keys[] = { 
		{  0.0f,	-15.63f, 0.0f, 0.0f}, // pos.z
		{ 10.0f,	-12.87f, 0.0f, 0.0f}, // pos.z
	};
	static const Pimp::AnimCurve::Pair camTestShape_rotX_keys[] = { 
		//{ 0.0000000f,	DEG2RAD(-28.0f), 0.0f, 0.0f},
		{ 5.0f,	DEG2RAD(-173.98f), 0.0f, 0.0f}, // rot.x
	};
	static const Pimp::AnimCurve::Pair camTestShape_rotY_keys[] = { 
		//{ 0.0000000f,	DEG2RAD(1182.0f), 0.0f, 0.0f},
		{ 5.0f, DEG2RAD(-43.63f), 0.0f, 0.0f}, // rot.y
	};
	static const Pimp::AnimCurve::Pair camTestShape_rotZ_keys[] = { 
		//{ 0.0000000f,	DEG2RAD(0.0f), 0.0f, 0.0f},
		{ 5.0f,	DEG2RAD(-141.09f), 0.0f, 0.0f}, // rot.z
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
	world->GetElements().Add(paramSpherePos_posX);
	world->GetElements().Add(paramSpherePos_posY);
	world->GetElements().Add(paramSpherePos_posZ);
	static const Pimp::AnimCurve::Pair paramSpherePos_posX_keys[] = { 
		{ 0.0000000f,	0.0f, 0.0f, 0.0f},
	};
	static const Pimp::AnimCurve::Pair paramSpherePos_posY_keys[] = { 
		{  0.0f,	-20.0f, 0.0f, 0.0f},
		{  2.0f,	-15.0f, 0.0f, 0.0f},
		{  4.0f,	-20.0f, 0.0f, 0.0f},
		{  6.0f,	-15.0f, 0.0f, 0.0f},
		{  8.0f,	-20.0f, 0.0f, 0.0f},
		{ 10.0f,	-15.0f, 0.0f, 0.0f},
		{ 12.0f,	-20.0f, 0.0f, 0.0f},
		{ 14.0f,	-15.0f, 0.0f, 0.0f},
		{ 16.0f,	-20.0f, 0.0f, 0.0f},
		{ 18.0f,	-15.0f, 0.0f, 0.0f},
	};
	static const Pimp::AnimCurve::Pair paramSpherePos_posZ_keys[] = { 
		{ 0.0000000f,	-3.3f, 0.0f, 0.0f},
	};
	paramSpherePos_posX->SetKeysPtr(paramSpherePos_posX_keys, sizeof(paramSpherePos_posX_keys)/sizeof(Pimp::AnimCurve::Pair));
	paramSpherePos_posY->SetKeysPtr(paramSpherePos_posY_keys, sizeof(paramSpherePos_posY_keys)/sizeof(Pimp::AnimCurve::Pair));
	paramSpherePos_posZ->SetKeysPtr(paramSpherePos_posZ_keys, sizeof(paramSpherePos_posZ_keys)/sizeof(Pimp::AnimCurve::Pair));

	xformParamSpherePos->SetAnimCurveTranslationX(paramSpherePos_posX);
	xformParamSpherePos->SetAnimCurveTranslationY(paramSpherePos_posY);
	xformParamSpherePos->SetAnimCurveTranslationZ(paramSpherePos_posZ);

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
	AddChildToParent(xformParamSpherePos,world->GetRootNode());
	AddChildToParent(paramSceneTime, world->GetRootNode());
	
	AddChildToParent(camTestShape,xformCamTest);
	AddChildToParent(paramSpherePos,xformParamSpherePos);


	// Camera direction (add all cameras)

	world->SetUseCameraDirection(true);
	FixedSizeList<Pimp::Camera*>& directionCameras = world->GetDirectionCameras();

	directionCameras.Add(camTestShape); // 0: test cam



	static const Pimp::AnimCurve::Pair cameraDirection_keys[] = { 
	 { -1.000000f,0.000000f,0.000000f,0.000000f},	// cam 0
	};
	world->GetCameraDirectionAnimCurve()->SetKeysPtr(cameraDirection_keys, sizeof(cameraDirection_keys)/sizeof(Pimp::AnimCurve::Pair));


	static const Pimp::AnimCurve::Pair sceneDirection_keys[] = { 
	 { -1.000000f,0.000000f,0.000000f,0.000000f},	// scene 0
	};
	world->GetSceneDirectionAnimCurve()->SetKeysPtr(sceneDirection_keys, sizeof(sceneDirection_keys)/sizeof(Pimp::AnimCurve::Pair));


	WaitForMaterialCompilationJobsToFinish();

	// Load textures

	Pimp::Texture2D* testTexture = LoadTexture(assetsPath + "testimage.png", true);
	world->GetTextures().Add(testTexture);


	// Init all materials, now that the shaders have been compiled.

	Pimp::Material* sceneShaderMat0 = new Pimp::Material(world, sceneShaderMat0_compiled_hlsl, sceneShaderMat0_compiled_hlsl_size, assetsPath + "scene_test.fx");
	world->GetMaterials().Add(sceneShaderMat0);
	sceneShader0->SetMaterial(sceneShaderMat0);
	world->GetScenes().Add(sceneShader0);
	world->GetElements().Add(sceneShader0);


	Pimp::Material* userPostEffectMat = new Pimp::Material(world, userPostEffectMat_compiled_hlsl, userPostEffectMat_compiled_hlsl_size, assetsPath + "posteffect.fx");
	world->GetMaterials().Add(userPostEffectMat);
	world->GetPostProcess()->SetUserPostEffect(userPostEffectMat);




	world->InitAllBalls();
	world->UpdateAllMaterialParameters();

	// Ready!
	
	*outWorld = world;
}
