
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
		Assets::AddTexture2D("textures\\2\\even_lachen.png", true, &texMesh);
		Assets::AddMaterial("shaders\\Scene_Ribbons2.fx", &ribbonMat);
	}

	void BindAnimationNodes()
	{
	}

	void BindAssets()
	{
		BindSceneMaterial(ribbonMat);
	}

	void Tick(double row)
	{
		SetSceneMaterial();
		SetMainSceneAndDefaultCamera();
	}
};
