
#pragma once

class Blobs : public Scene
{
private:
	Pimp::Texture2D *bgTile;
	Pimp::Texture2D *envMap, *projMap;

public:
	Blobs()
	{
	}

	~Blobs()
	{
	}

	void ReqRocketTracks()
	{
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\creds\\tile-00.png", &bgTile);
		Assets::AddTexture2D("textures\\creds\\envmap.png", &envMap);
		Assets::AddTexture2D("textures\\creds\\projmap.png", &projMap);
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
			Pimp::D3D::BlendMode::BM_None, 
			-1, 
			Vector2(0.f, 0.f), 
			Vector2(1920.f, 1080.f), 
			Vector2(3.f*kTileMul, 3.f),
			Vector2(-time*0.4f, -time));

		// FIXME: parametrize w/Rocket?
		Quaternion rotation = CreateQuaternionFromYawPitchRoll(time*0.6f, time*0.8f, time*0.4f);
		s_pMetaballs->SetRotation(rotation);

		// FIXME: make it look interesting (attractors?)
		for (unsigned int iBall4 = 0; iBall4 < kNumMetaball4s; ++iBall4)
		{
			for (unsigned int iBall = 0; iBall < 4; ++iBall)
			{
				const unsigned int ballCnt = iBall4*4 + iBall;
				s_metaball4s[iBall4].X[iBall] = 0.5f*sinf(ballCnt + time*1.1f);
				s_metaball4s[iBall4].Y[iBall] = 0.5f*sinf((ballCnt^7) + time*1.35f);
				s_metaball4s[iBall4].Z[iBall] = 0.5f*cosf((ballCnt^5) + time*1.4f);
			}
		}

		// Generate geometry (triggers visibility).
		s_pMetaballs->Generate(kNumMetaball4s, s_metaball4s, 190.f);
		
		// Set maps & lighting.
		s_pMetaballs->SetMaps(
			envMap, projMap,
			(float) sync_get_val(st_blobsProjScrollU, row),
			(float) sync_get_val(st_blobsProjScrollV, row));
		s_pMetaballs->SetLighting(
			(float) sync_get_val(st_blobsShininess, row),
			(float) sync_get_val(st_blobsOverbright, row));
	}
};
