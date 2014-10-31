
class GeneralCinema : public Demo::Scene
{
private:
	Pimp::Texture2D *projector, *reel, *scroller;
	Pimp::Material *backMat;

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

		Assets::AddMaterial("shaders\\Scene_Empty.fx", &backMat);
	}

	void BindToWorld()
	{
		BindSceneMaterial(backMat);
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();
		s_sprites->AddBackgroundSprite(Pimp::gD3D->GetWhiteTex(), Pimp::D3D::BlendMode::BM_None, 0, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));

		const float kProjectorZ = 1.f;
		const float kReelZ = 2.f;
		const float kScrollZ = 3.f;

		const float yOffs = -50.f;
		const float reelX = -3.f;

		const float reelRoto = (float) sync_get_val(st_fxTime, row);

		// "projector"
		s_sprites->AddSprite(
				projector,
				Pimp::D3D::BlendMode::BM_Additive,
				Vector2(100.f, 370.f+yOffs),
				kProjectorZ,
				0.7f,
				0.f);

		// reels
		s_sprites->AddSprite(
				reel,
				Pimp::D3D::BlendMode::BM_Additive,
				Vector2(480.f+reelX, 100.f+yOffs),
				kProjectorZ,
				0.7f,
				reelRoto*2.f);
		s_sprites->AddSprite(
				reel,
				Pimp::D3D::BlendMode::BM_Additive,
				Vector2(480.f+reelX, 650.f+yOffs),
				kProjectorZ,
				0.7f,
				-reelRoto*2.f);
	}
};
