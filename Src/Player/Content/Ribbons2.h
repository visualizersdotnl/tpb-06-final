
class Ribbons2 : public Scene
{
private:
	Pimp::Texture2D *texWall, *texMesh;
	Pimp::Material *ribbonMat;

public:
	Ribbons2()
	{
	}

	~Ribbons2()
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
		Assets::AddTexture2D("textures\\ribbons2\\even_lachen.png", &texMesh);
		Assets::AddMaterial("shaders\\Scene_Ribbons2.fx", &ribbonMat);
	}

	void BindToWorld()
	{
		BindSceneMaterial(ribbonMat);
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();
		s_sprites->AddBackgroundSprite(texWhite, Pimp::D3D::BlendMode::BM_None, 0, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));
	}
};
