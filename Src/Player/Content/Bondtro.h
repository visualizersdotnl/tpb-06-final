
class Bondtro : public Demo::Scene
{
private:
	Pimp::Texture2D *blaxMister;

public:
	Bondtro()
	{
	}

	~Bondtro()
	{
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\ribbons_mesh.png", true, &blaxMister);
	}

	void BindAnimationNodes()
	{
	}

	void BindAssets()
	{
//		SetSceneMaterial(ribbonMat);
	}

	void Tick()
	{
//		SetMainSceneAndDefaultCamera();
		s_sprites->AddSprite(blaxMister, Pimp::D3D::BlendMode::BM_Additive, -1, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), 1.f, 0.f);
	}
};
