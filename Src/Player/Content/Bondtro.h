
class Bondtro : public Demo::Scene
{
private:
	Pimp::Texture2D *bondBlob;

public:
	Bondtro()
	{
	}

	~Bondtro()
	{
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\1\\bond-circle.png", true, &bondBlob); // 106*106
	}

	void BindAnimationNodes()
	{
	}

	void BindAssets()
	{
//		SetSceneMaterial(ribbonMat);
	}

	void Tick(double row)
	{
//		SetMainSceneAndDefaultCamera();

//		const float aspectRatio = Pimp::Configuration::Instance()->GetRenderAspectRatio();
		const float ballPos = sync_get_val(st_bondBlob, row);
		const float ballY = 1080.f*0.5f-(bondBlob->GetHeight()*0.5f);
	
		s_sprites->AddSprite(
				bondBlob,
				Pimp::D3D::BlendMode::BM_Additive,
				Vector2(ballPos, ballY),
				1.f);

	}
};
