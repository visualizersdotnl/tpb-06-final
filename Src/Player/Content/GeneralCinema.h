
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

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\5\\greetings_projector.png", false, &projector);
		Assets::AddTexture2D("textures\\5\\greetings_projector_reel.png", false, &reel);
		Assets::AddTexture2D("textures\\5\\greetings_projector_scroller.png", false, &scroller);
		Assets::AddMaterial("shaders\\Spheregrid.fx", &backMat);
	}

	void BindAnimationNodes()
	{
	}

	void BindAssets()
	{
		BindSceneMaterial(backMat);
	}

	void Tick(double row)
	{
		SetSceneMaterial();
		SetMainSceneAndDefaultCamera();

		const float kProjectorZ = 1.f;
		const float kReelZ = 2.f;
		const float kScrollZ = 3.f;

		const float yOffs = -50.f;
		const float reelX = -3.f;

		const float reelRoto = (float) sync_get_val(st_fxTimeGlobal, row);

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
				reelRoto*2.1f);
	}
};
