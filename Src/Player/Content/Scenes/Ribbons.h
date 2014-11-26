
class Ribbons : public Scene
{
private:
	Pimp::Texture2D *texWall, *texMesh, *texBack;
	Pimp::Material *ribbonMat;

	Pimp::Texture2D *envMap, *projMap;

	Pimp::Texture2D *logo;

	const sync_track *st_logoY;
	const sync_track *st_logoAlpha;

public:
	Ribbons()
	{
	}

	~Ribbons()
	{
	}

	void ReqRocketTracks()
	{
		s_syncTracks.push_back(SyncTrack("ribbonsAppear", true));	
		s_syncTracks.push_back(SyncTrack("ribbonsPhase", true));	
		s_syncTracks.push_back(SyncTrack("ribbonsWonkyness", true));		

		s_syncTracks.push_back(SyncTrack("mainLogoY", false, &st_logoY));
		s_syncTracks.push_back(SyncTrack("mainLogoAlpha", false, &st_logoAlpha));
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\ribbons1\\even_lachen.png", &texMesh);
		Assets::AddTexture2D("textures\\ribbons1\\background.png", &texBack);
		Assets::AddTexture2D("textures\\ribbons1\\tpb-06_2.png", &logo);

		Assets::AddMaterial("shaders\\Scene_Ribbons.fx", &ribbonMat);
	}

	void BindToWorld()
	{
		BindSceneMaterial(ribbonMat);
		ribbonMat->SetBlendMode(Pimp::D3D::Blend::BM_AlphaBlend);
	}

	void Tick(double row)
	{
		SetMainSceneAndCamera(s_defaultCam);
		s_sprites->AddBackgroundSprite(texBack, Pimp::D3D::Blend::BM_None, -1, 1.f, true);

		float logoY = (float) sync_get_val(st_logoY, row);
		float logoA = (float) sync_get_val(st_logoAlpha, row);
		s_sprites->AddSprite(logo, Pimp::D3D::Blend::BM_AlphaBlend, Vector2(0.f, logoY), 1.f, logoA, 0.f, true); 
	}
};
