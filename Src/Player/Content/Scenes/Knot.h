
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
		SetMainSceneAndCamera(s_defaultCam);
		s_sprites->AddBackgroundSprite(texWhite, Pimp::D3D::Blend::BM_None, 0, 1.f, true);
	}
};
