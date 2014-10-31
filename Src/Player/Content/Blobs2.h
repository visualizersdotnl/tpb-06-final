
#pragma once

class Blobs2 : public Scene
{
private:
	Pimp::Texture2D *bgTile;

public:
	Blobs2()
	{
	}

	~Blobs2()
	{
	}

	void ReqRocketTracks()
	{
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\bgtiles\\tile-00-yellow.png", &bgTile);
	}

	void BindToWorld()
	{
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();

		float time = (float) sync_get_val(st_fxTime, row);
			
		s_sprites->AddBackgroundSprite(
			bgTile, 
			Pimp::D3D::BlendMode::BM_AlphaBlend, 
			-1, 
			Vector2(0.f, 0.f), 
			Vector2(1920.f, 1080.f), 
			Vector2(4.f*kTileMul, 4.f),
			Vector2(time*0.1f, time));

		// FIXME: parametrize w/Rocket
		Quaternion rotation = CreateQuaternionFromYawPitchRoll(time*0.6f, time*0.8f, time*0.4f);
		s_pMetaballs->SetRotation(rotation);

		// FIXME: make it look interesting (attractors?)
		for (unsigned int iBall4 = 0; iBall4 < kNumMetaball4s; ++iBall4)
		{
			for (unsigned int iBall = 0; iBall < 4; ++iBall)
			{
				const unsigned int ballCnt = iBall4*4 + iBall;
				s_metaball4s[iBall4].X[iBall] = 0.6f*sinf(ballCnt + time*0.9f);
				s_metaball4s[iBall4].Y[iBall] = 0.5f*sinf((ballCnt^9) + time*1.15f);
				s_metaball4s[iBall4].Z[iBall] = 0.6f*cosf((ballCnt^4) + time*1.7f);
			}
		}

		// Generate geometry (triggers visibility).
		s_pMetaballs->Generate(kNumMetaball4s, s_metaball4s, 190.f);
	}
};
