
class GeneralCinema : public Demo::Scene
{
private:
	Pimp::Texture2D *projector, *reel, *scroller;
	Pimp::Material *backMat;
	Pimp::MaterialParameter *foldXformParam;

	const sync_track *st_greetsRotX;
	const sync_track *st_greetsRotY;
	const sync_track *st_greetsRotZ;
	const sync_track *st_greetsPosX;
	const sync_track *st_greetsPosY;
	const sync_track *st_greetsPosZ;

public:
	GeneralCinema()
	{
	}

	~GeneralCinema()
	{
	}

	void ReqRocketTracks()
	{
		s_syncTracks.push_back(SyncTrack("greetsRotX", false, &st_greetsRotX));
		s_syncTracks.push_back(SyncTrack("greetsRotY", false, &st_greetsRotY));
		s_syncTracks.push_back(SyncTrack("greetsRotZ", false, &st_greetsRotZ));
		s_syncTracks.push_back(SyncTrack("greetsPosX", false, &st_greetsPosX));
		s_syncTracks.push_back(SyncTrack("greetsPosY", false, &st_greetsPosY));
		s_syncTracks.push_back(SyncTrack("greetsPosZ", false, &st_greetsPosZ));
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\generalcinema\\greetings_projector.png", &projector);
		Assets::AddTexture2D("textures\\generalcinema\\greetings_projector_reel.png", &reel);
		Assets::AddTexture2D("textures\\generalcinema\\greetings_projector_scroller.png", &scroller);

		Assets::AddMaterial("shaders\\Scene_Tentacle.fx", &backMat);
	}

	void BindToWorld()
	{
		BindSceneMaterial(backMat);

		foldXformParam = AddMaterialParamWithXform("foldXformInv", true);
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();
		s_sprites->AddBackgroundSprite(texWhite, Pimp::D3D::BlendMode::BM_None, 0, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));

		const float kProjectorZ = 1.f;
		const float kReelZ = 2.f;
		const float kScrollZ = 3.f;

		// "projector"
		s_sprites->AddSprite(
				projector,
				Pimp::D3D::BlendMode::BM_Additive,
				Vector2(100.f, (1080.f-439.f)*0.5f),
				kProjectorZ,
				1.f,
				0.f);

		const float kReelX = 484.f;
		const float kReelY = (1080.f-439.f)*0.5f;
		const float kReelYOffs = 35.f;

		const float reelRoto = (float) sync_get_val(st_fxTime, row);

		// reels (rotate quickly in same direction in orig. bumper)
		s_sprites->AddSprite(
				reel,
				Pimp::D3D::BlendMode::BM_Additive,
				Vector2(kReelX, kReelY-(reel->GetHeight()/2)-kReelYOffs),
				kProjectorZ,
				1.f,
				-reelRoto*2.f);
		s_sprites->AddSprite(
				reel,
				Pimp::D3D::BlendMode::BM_Additive,
				Vector2(kReelX, kReelY+(reel->GetHeight()/2)+kReelYOffs+7.f), // @plek: Lower this one a few pixels.
				kProjectorZ,
				1.f,
				-reelRoto*2.f);


		// Update inv param
		const float rotationX = (float) sync_get_val(st_greetsRotX, row);
		const float rotationY = (float) sync_get_val(st_greetsRotY, row);
		const float rotationZ = (float) sync_get_val(st_greetsRotZ, row);
		const float positionX = (float) sync_get_val(st_greetsPosX, row);
		const float positionY = (float) sync_get_val(st_greetsPosY, row);
		const float positionZ = (float) sync_get_val(st_greetsPosZ, row);
		Quaternion rotation = CreateQuaternionFromYawPitchRoll(rotationX, rotationY, rotationZ);
		((Pimp::Xform*)foldXformParam->GetParents()[0])->SetRotation(rotation);
		((Pimp::Xform*)foldXformParam->GetParents()[0])->SetTranslation(Vector3(positionX, positionY, positionZ));
	}
};
