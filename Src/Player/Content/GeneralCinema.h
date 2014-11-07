
class GeneralCinema : public Demo::Scene
{
private:
	Pimp::Texture2D *projector, *reel, *background;
	Pimp::Material *backMat;
	Pimp::MaterialParameter *foldXformParam;
	Pimp::Texture2D *flare;

	const sync_track *st_greetsRotX;
	const sync_track *st_greetsRotY;
	const sync_track *st_greetsRotZ;
	const sync_track *st_greetsPosX;
	const sync_track *st_greetsPosY;
	const sync_track *st_greetsPosZ;

	const sync_track *st_greetsFlare;
	const sync_track *st_greetsBitmap;

	Pimp::Texture2D *genbitmap[7];

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
	
		s_syncTracks.push_back(SyncTrack("greetsFlare", false, &st_greetsFlare));
		s_syncTracks.push_back(SyncTrack("greetsBitmap", false, &st_greetsBitmap));
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\generalcinema\\greetings_projector.png", &projector);
		Assets::AddTexture2D("textures\\generalcinema\\greetings_projector_reel.png", &reel);
		Assets::AddTexture2D("textures\\generalcinema\\tentacles_pattern.png", NULL);
		Assets::AddTexture2D("textures\\generalcinema\\tentacles_noise.png", NULL);
		Assets::AddTexture2D("textures\\generalcinema\\background.png", &background);
		Assets::AddTexture2D("textures\\generalcinema\\Flare.png", &flare);

		Assets::AddTexture2D("textures\\generalcinema\\greetings_projector_scroller1.png", &genbitmap[0]);
		Assets::AddTexture2D("textures\\generalcinema\\greetings_projector_scroller2.png", &genbitmap[1]);
		Assets::AddTexture2D("textures\\generalcinema\\greetings_projector_scroller3.png", &genbitmap[2]);
		Assets::AddTexture2D("textures\\generalcinema\\greetings_projector_scroller4.png", &genbitmap[3]);
		Assets::AddTexture2D("textures\\generalcinema\\greetings_projector_scroller5.png", &genbitmap[4]);
		Assets::AddTexture2D("textures\\generalcinema\\greetings_projector_scroller6.png", &genbitmap[5]);
		Assets::AddTexture2D("textures\\generalcinema\\greetings_projector_scroller7.png", &genbitmap[6]);

		Assets::AddMaterial("shaders\\Scene_Tentacle.fx", &backMat);	
	}

	void BindToWorld()
	{
		BindSceneMaterial(backMat);
		backMat->SetBlendMode(Pimp::D3D::BlendMode::BM_AlphaBlend);

		foldXformParam = AddMaterialParamWithXform("foldXformInv", true);
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();
		s_sprites->AddBackgroundSprite(background, Pimp::D3D::BlendMode::BM_None, -1, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));

		const float kProjectorZ = 1.f;
		const float kReelZ = 2.f;
		const float kFlareZ = 3.f;
		const float kScrollZ = 4.f;

		// the following code is pure manual fucking around to align sprites
		// there is no real logic to it

		// "projector" 
		s_sprites->AddSprite(
				projector,
				Pimp::D3D::BlendMode::BM_Additive,
				Vector2(100.f, (1080.f-projector->GetHeight())*0.5f),
				kProjectorZ,
				1.f,
				0.f);

		const float kReelX = 345.f;
		const float kReelY = (1080.f-projector->GetHeight())*0.5f;

		const float reelRoto = (float) sync_get_val(st_fxTime, row);

		// stationary position (opening forward, like in 80s bumper)
		const float reelStat = -M_PI*0.5f;

		// reels (rotate quickly in opposite directions in orig. bumper)
		s_sprites->AddSprite(
				reel,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(kReelX, kReelY-(reel->GetHeight()/2)-75.f),
				kReelZ,
				1.f,
				reelStat + reelRoto*1.75f, true);
		s_sprites->AddSprite(
				reel,
				Pimp::D3D::BlendMode::BM_Additive,
				Vector2(kReelX, kReelY+(reel->GetHeight()/2)+45.f),
				kReelZ,
				1.f,
				reelStat + -reelRoto*1.75f, true);

		const float flareSync = (float) sync_get_val(st_greetsFlare, row);

		const float kFlareX = kReelX - 360.f;

		if (0.f != flareSync)
		{
			float flareXOffs = 0.f;
			flareXOffs += flareSync*1700.f;

			s_sprites->AddSprite(
				flare,
				Pimp::D3D::BlendMode::BM_Additive,
				Vector2(kFlareX + flareXOffs, (1080.f-flare->GetHeight())*0.5f),
				kFlareZ,
				flareSync,
				0.f, true);
		}

		const float kProjOffsX = 100.f + projector->GetWidth() + 10.f;

		// greet shit

		const float fBmp = (float) sync_get_val(st_greetsBitmap, row);
		if (fBmp >= 1.f)
		{
			const int iBmp = (int) floorf(fBmp) - 1;
			
			float aBmp = 1.f;

			float x = fmodf(fBmp, 1.f);
			if (x < 0.25f) aBmp = x*4.f;
			if (x > 0.75f) aBmp = 1.f-((x-0.75f)*4.f);

			{
				Pimp::Texture2D *bitmap = genbitmap[iBmp];
				s_sprites->AddSprite(
					bitmap,
					Pimp::D3D::BlendMode::BM_Additive,
					Vector2(kProjOffsX, (1080.f-bitmap->GetHeight())*0.5f),
					kScrollZ,
					aBmp,
					0.f,
					true);
			}
		}

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
