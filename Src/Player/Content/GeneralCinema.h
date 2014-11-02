
class GeneralCinema : public Demo::Scene
{
private:
	Pimp::Texture2D *projector, *reel, *scroller;
//	Pimp::Material *backMat;

public:
	GeneralCinema()
	{
	}

	~GeneralCinema()
	{
	}

	void ReqRocketTracks()
	{
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\generalcinema\\greetings_projector.png", &projector);
		Assets::AddTexture2D("textures\\generalcinema\\greetings_projector_reel.png", &reel);
		Assets::AddTexture2D("textures\\generalcinema\\greetings_projector_scroller.png", &scroller);

//		Assets::AddMaterial("shaders\\Scene_IQ.fx", &backMat);
	}

	void BindToWorld()
	{
//		BindSceneMaterial(backMat);
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
	}
};
