
class Pompom : public Scene
{
private:
	Pimp::Material *sceneMat;

public:
	Pompom()
	{
	}

	~Pompom()
	{
	}

	void ReqRocketTracks()
	{
	}

	void ReqAssets()
	{
		Assets::AddMaterial("shaders\\Scene_Pompom.fx", &sceneMat);
	}

	void BindToWorld()
	{
		BindSceneMaterial(sceneMat);
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();
		s_sprites->AddBackgroundSprite(texWhite, Pimp::D3D::BlendMode::BM_None, 0, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));
	}
};
