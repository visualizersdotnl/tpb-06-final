
class Ribbons : public Scene
{
private:
	Pimp::Texture2D *texWall, *texMesh, *texBack;
	Pimp::Material *ribbonMat;

	Pimp::Texture2D *envMap, *projMap;

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
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\ribbons1\\even_lachen.png", &texMesh);
		Assets::AddTexture2D("textures\\ribbons1\\background.png", &texBack);
		Assets::AddMaterial("shaders\\Scene_Ribbons.fx", &ribbonMat);
	}

	void BindToWorld()
	{
		BindSceneMaterial(ribbonMat);
		ribbonMat->SetBlendMode(Pimp::D3D::BlendMode::BM_AlphaBlend);
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();
		s_sprites->AddBackgroundSprite(texBack, Pimp::D3D::BlendMode::BM_None, -1, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));
	}
};
