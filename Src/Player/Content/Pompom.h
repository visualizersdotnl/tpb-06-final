
class Pompom : public Scene
{
private:
	Pimp::Material *sceneMat;
	Pimp::Texture2D *background;

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
		Assets::AddTexture2D("textures\\pompom\\background.png", &background);		
		Assets::AddTexture2D("textures\\pompom\\pompom_noise.png", NULL);
		Assets::AddTexture2D("textures\\pompom\\pompom_color.png", NULL);
		Assets::AddMaterial("shaders\\Scene_Pompom.fx", &sceneMat);
	}

	void BindToWorld()
	{
		BindSceneMaterial(sceneMat);
		sceneMat->SetBlendMode(Pimp::D3D::BlendMode::BM_AlphaBlend);
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();
		s_sprites->AddBackgroundSprite(background, Pimp::D3D::BlendMode::BM_None, -1, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));
	}
};
