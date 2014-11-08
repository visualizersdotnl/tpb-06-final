
#pragma once

class Blobs2 : public Scene
{
private:
	Pimp::Texture2D *bgTile;
	Pimp::Texture2D *toy[8];
	Pimp::Texture2D *tileshadow;
	
	const sync_track *st_toy;

	Pimp::Texture2D *envMap, *projMap;

public:
	Blobs2()
	{
	}

	~Blobs2()
	{
	}

	void ReqRocketTracks()
	{
		s_syncTracks.push_back(SyncTrack("toypusher", false, &st_toy));
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\toypusher\\tile-00-yellow.png", &bgTile);

		Assets::AddTexture2D("textures\\toypusher\\1.png", &toy[0]);
		Assets::AddTexture2D("textures\\toypusher\\1b.png", &toy[1]);
		Assets::AddTexture2D("textures\\toypusher\\2.png", &toy[2]);
		Assets::AddTexture2D("textures\\toypusher\\2b.png", &toy[3]);
		Assets::AddTexture2D("textures\\toypusher\\3.png", &toy[4]);
		Assets::AddTexture2D("textures\\toypusher\\3b.png", &toy[5]);
		Assets::AddTexture2D("textures\\toypusher\\4.png", &toy[6]);
		Assets::AddTexture2D("textures\\toypusher\\4b.png", &toy[7]);
		Assets::AddTexture2D("textures\\toypusher\\tileshadow.png", &tileshadow);		

		Assets::AddTexture2D("textures\\toypusher\\envmap.png", &envMap);
		Assets::AddTexture2D("textures\\toypusher\\projmap.png", &projMap);
	}

	void BindToWorld()
	{
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();

		float time = (float) sync_get_val(st_fxTime, row);
			
		s_sprites->AddBackgroundSprite(
			0,
			bgTile, 
			Pimp::D3D::BlendMode::BM_AlphaBlend, 
			-1, 
			Vector2(0.f, 0.f), 
			Vector2(1920.f, 1080.f), 
			Vector2(3.f*kTileMul, 3.f),
			Vector2(time*0.4f, time));

		s_sprites->SkipBGSprite2(true);

		// Toypusher stuff
		const float toySync = (float) sync_get_val(st_toy, row);
		const float kToyZ = 1.f;
		const float kToySpacing = 128.f;
		const float kToyOffsY = (1080.f-(8.f*kToySpacing))/2.f;

		const float kShadowX = -20.0f;
		const float kShadowY =  20.0f;

		// shadows
		for (int iToy = 0; iToy < 8; ++ iToy)
		{
			if (toySync >= (float) iToy)
			{
				const float iToy1 = 1.f + iToy;
				float alpha = (toySync >= iToy1) ? 1.f : toySync-iToy;
				float rotZ = 0.25f-(alpha*0.25f);

				s_sprites->AddSprite(
					tileshadow, 
					Pimp::D3D::BlendMode::BM_AlphaBlend,
					Vector2(125.f + kShadowX, kShadowY + kToyOffsY + kToySpacing*iToy), 
					kToyZ,
					alpha,
					rotZ);
			}
		}

		// tiles
		for (int iToy = 0; iToy < 8; ++ iToy)
		{
			if (toySync >= (float) iToy)
			{
				const float iToy1 = 1.f + iToy;
				float alpha = (toySync >= iToy1) ? 1.f : toySync-iToy;
				float rotZ = 0.25f-(alpha*0.25f);

				s_sprites->AddSprite(
					toy[iToy], 
					Pimp::D3D::BlendMode::BM_AlphaBlend,
					Vector2(125.f, kToyOffsY + kToySpacing*iToy), 
					kToyZ,
					alpha,
					rotZ);
			}
		}

		// FIXME: parametrize w/Rocket?
		Quaternion rotation = CreateQuaternionFromYawPitchRoll(-time*0.6f, time*0.7f, time*0.5f);
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
		s_pMetaballs->Generate(kNumMetaball4s, s_metaball4s, 170.f);

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
