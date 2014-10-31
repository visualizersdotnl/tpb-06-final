
class Ribbons : public Scene
{
private:
	Pimp::Texture2D *texWall, *texMesh;
	Pimp::Material *ribbonMat;

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
		Assets::AddMaterial("shaders\\Scene_Ribbons.fx", &ribbonMat);
	}

	void BindToWorld()
	{
		BindSceneMaterial(ribbonMat);
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();
		s_sprites->AddBackgroundSprite(Pimp::gD3D->GetWhiteTex(), Pimp::D3D::BlendMode::BM_None, 0, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));
	}
};
