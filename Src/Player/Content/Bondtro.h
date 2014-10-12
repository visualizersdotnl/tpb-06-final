
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

		const float assRat = Pimp::Configuration::Instance()->GetRenderAspectRatio();
		float bal_1 = syncTracks[iBondBlobs+0].Get(row);
//		float bal_2 = syncTracks[iBondBlobs+1].Get(row);
//		float bal_3 = syncTracks[iBondBlobs+2].Get(row);
			
		const float blob_hoogte = 1080.f*0.5f-(bondBlob->GetHeight()*0.5f);

	
		s_sprites->AddSprite(
				bondBlob,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(bal_1, blob_hoogte),
				1.f);

#if 0
		if (balfade_1<=1.f)
		{
			s_sprites->AddSprite(
					bondBlob,
					Pimp::D3D::BlendMode::BM_Additive,
					Vector2(bal_2, blob_hoogte),
					1.f,
					1.f);
		}

		s_sprites->AddSprite(
				bondBlob,
				Pimp::D3D::BlendMode::BM_Additive,
				Vector2(bal_3, blob_hoogte),
				1.f,
				1.f);
		}
#endif
	}
};
