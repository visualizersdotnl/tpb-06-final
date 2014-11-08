
#pragma once

class Blobs : public Scene
{
private:
	Pimp::Texture2D *bgTile;
	Pimp::Texture2D *envMap, *projMap;

	Pimp::Texture2D *alien, *alien_a, *guy, *guy_a, *glow, *glow_a, *plek, *plek_a, *punqtured, *punqtured_a, 
	                *shifter, *shifter_a;

	const sync_track *st_credit, *st_creditX, *st_creditY;

public:
	Blobs()
	{
	}

	~Blobs()
	{
	}

	void ReqRocketTracks()
	{
		s_syncTracks.push_back(SyncTrack("credit", false, &st_credit));
		s_syncTracks.push_back(SyncTrack("creditX", false, &st_creditX));
		s_syncTracks.push_back(SyncTrack("creditY", false, &st_creditY));
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\creds\\tile-00.png", &bgTile);
		Assets::AddTexture2D("textures\\creds\\envmap.png", &envMap);
		Assets::AddTexture2D("textures\\creds\\projmap.png", &projMap);

		Assets::AddTexture2D("textures\\creds\\alien.png", &alien);
		Assets::AddTexture2D("textures\\creds\\alien-a.png", &alien_a);
		Assets::AddTexture2D("textures\\creds\\glow.png", &glow);
		Assets::AddTexture2D("textures\\creds\\glow-a.png", &glow_a);
		Assets::AddTexture2D("textures\\creds\\shifter.png", &shifter);
		Assets::AddTexture2D("textures\\creds\\shifter-a.png", &shifter_a);
		Assets::AddTexture2D("textures\\creds\\plek.png", &plek);
		Assets::AddTexture2D("textures\\creds\\plek-a.png", &plek_a);
		Assets::AddTexture2D("textures\\creds\\punqtured.png", &punqtured);
		Assets::AddTexture2D("textures\\creds\\punctured-a.png", &punqtured_a);
		Assets::AddTexture2D("textures\\creds\\guy.png", &guy);
		Assets::AddTexture2D("textures\\creds\\guy-a.png", &guy_a);
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
			Pimp::D3D::BlendMode::BM_None, 
			-1, 
			Vector2(0.f, 0.f), 
			Vector2(1920.f, 1080.f), 
			Vector2(3.f*kTileMul, 3.f),
			Vector2(-time*0.4f, -time));

		s_sprites->SkipBGSprite2(true);

		// Credits

		const float kCredZ = 1.f;

		Pimp::Texture2D *first, *second;
		first = second = nullptr;
	
		const int credit = (int) sync_get_val(st_credit, row);
		
		switch (credit)
		{
		case 1:	
			first = guy_a;
			second = guy;
			break;

		case 2:	
			first = glow_a;
			second = glow;
			break;

		case 3:	
			first = shifter_a;
			second = shifter;
			break;

		case 4:	
			first = plek_a;
			second = plek;
			break;

		case 5:	
			first = alien_a;
			second = alien;
			break;

		case 6:	
			first = punqtured_a;
			second = punqtured;
			break;
		}

		if (first != nullptr)
		{
			const float first_offs = (float) sync_get_val(st_creditX, row);
			const float second_offs = (float) sync_get_val(st_creditY, row);

			float xoffs1 = 0.f;
			float yoffs1 = 0.f;
			float xoffs2 = 0.f;
			float yoffs2 = 0.f;

			xoffs1 = first_offs*1920.f;
			yoffs1 = -first_offs*(1080.f);

			xoffs2 = second_offs*1920.f;
			yoffs2 = -second_offs*(1080.f);

			unsigned int fade = AlphaToVtxColor(1.f);

			s_sprites->AddSprite(
				first,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				fade,
				Vector2(xoffs1, yoffs1),
				Vector2(1920.f, 1080.f),
				kCredZ,
				0.f,
				true);

			s_sprites->AddSprite(
				second,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				fade,
				Vector2(xoffs2, yoffs2),
				Vector2(1920.f, 1080.f),
				kCredZ+0.1f,
				0.f,
				true);
		}

		// FIXME: parametrize w/Rocket?
//		Quaternion rotation = CreateQuaternionFromYawPitchRoll(0.f, 0.f, 0.f);
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
		s_pMetaballs->Generate(kNumMetaball4s, s_metaball4s, 200.f);
//		s_pMetaballs->Generate(kNumMetaball4s, s_metaball4s, 190.f);
		
		// Set maps & lighting.
		s_pMetaballs->SetMaps(
			envMap, projMap,
			(float) sync_get_val(st_blobsProjScrollU, row),
			(float) sync_get_val(st_blobsProjScrollV, row));
		s_pMetaballs->SetLighting(
			(float) sync_get_val(st_blobsShininess, row),
			(float) sync_get_val(st_blobsOverbright, row));
		s_pMetaballs->SetRim(0.4f);
	}
};
