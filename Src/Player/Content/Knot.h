
class Knot : public Scene
{
private:
	Pimp::Texture2D *texWall, *texMesh;
	Pimp::Material *sceneMat;

public:
	Knot()
	{
	}

	~Knot()
	{
	}

	void ReqRocketTracks()
	{
		s_syncTracks.push_back(SyncTrack("knotTubeRadius1", true));
		s_syncTracks.push_back(SyncTrack("knotTubeRadius2", true));
		s_syncTracks.push_back(SyncTrack("knotTubeRadius3", true));
	}

	void ReqAssets()
	{
		Assets::AddMaterial("shaders\\Scene_Knot.fx", &sceneMat);
	}

	void BindToWorld()
	{
		BindSceneMaterial(sceneMat);
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();
		s_sprites->AddBackgroundSprite(Pimp::gD3D->GetWhiteTex(), Pimp::D3D::BlendMode::BM_None, 0, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));
	}
};
