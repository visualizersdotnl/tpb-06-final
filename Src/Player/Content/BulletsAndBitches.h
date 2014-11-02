
class BulletsAndBitches : public Demo::Scene
{
private:
	Pimp::Texture2D *background;

	const sync_track *st_bitchBullets;
public:
	BulletsAndBitches()
	{
	}

	~BulletsAndBitches()
	{
	}
	
	void ReqRocketTracks()
	{
		s_syncTracks.push_back(SyncTrack("bitchBullets", false, &st_bitchBullets));
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\loading.png", &background);
	}

	void BindToWorld()
	{
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();
		s_sprites->AddBackgroundSprite(texWhite, Pimp::D3D::BlendMode::BM_None, 0, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));

		// background (or what used to be our loading screen)
		s_sprites->AddSprite(
				background,
				Pimp::D3D::BlendMode::BM_Additive,
				AlphaToVtxColor(1.f),
				Vector2(0.f, 0.f), Vector2(1920.f, 1080.f),
				1.f,
				0.f);
	}
};
