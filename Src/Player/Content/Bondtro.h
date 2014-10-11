
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
		s_sprites->AddSpriteCenter(blaxMister, Pimp::D3D::BlendMode::BM_Additive, Vector2(0.f, 0.f), 1.f, 1.f);
	}
};
