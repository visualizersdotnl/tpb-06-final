
class BulletsAndBitches : public Demo::Scene
{
private:
	Pimp::Texture2D *background, *target, *blast[3], *pimp, *blob, *background2;
	Pimp::Texture2D *thunderball;
		 
	const sync_track *st_bitchBullets;

	const sync_track *st_endFlangerWet;
	const sync_track *st_endFlangerFreqMod;


public:
	BulletsAndBitches()
	{
	}

	~BulletsAndBitches()
	{
	}
	
	void ReqRocketTracks()
	{
		s_syncTracks.push_back(SyncTrack("bitchBullets", false, &st_bitchBullets));
		s_syncTracks.push_back(SyncTrack("endFlangerWet", false, &st_endFlangerWet));
		s_syncTracks.push_back(SyncTrack("endFlangerFreqMod", false, &st_endFlangerFreqMod));
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\bitches\\loading.png", &background);
		Assets::AddTexture2D("textures\\bitches\\thunderball.png", &thunderball);

		// shared w/Bondtro
		Assets::AddTexture2D("textures\\bondtro\\bond-bullseye.png", &target);
		Assets::AddTexture2D("textures\\bondtro\\bond-hole.png", &blast[0]);
		Assets::AddTexture2D("textures\\bondtro\\bond-hole-2.png", &blast[1]);
		Assets::AddTexture2D("textures\\bondtro\\bond-hole-3.png", &blast[2]);
		Assets::AddTexture2D("textures\\bondtro\\bond-meneer.png", &pimp);
		Assets::AddTexture2D("textures\\bondtro\\bond-circle.png", &blob);
		Assets::AddTexture2D("textures\\bondtro\\bg.png", &background2);


	}

	void BindToWorld()
	{
	}

	void EndPic(float alpha)
	{
		SetMainSceneAndDefaultCamera();
		s_sprites->AddBackgroundSprite(thunderball, Pimp::D3D::BlendMode::BM_Additive, AlphaToVtxColor(alpha), Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));
	}

	void Tick(double row)
	{
		const float wetDry = (float) sync_get_val(st_endFlangerWet, row);
		const float freqMod = (float) sync_get_val(st_endFlangerFreqMod, row);
		Audio_FlangerMP3(wetDry, freqMod);
		
		SetMainSceneAndDefaultCamera();

		const float bitchBullets = (float) sync_get_val(st_bitchBullets, row);

		// if this is still zero we're not blasting the guns yet
		if (bitchBullets == 0.f)
		{
			// this way it's part of the scene process and thus we can cover it with noise
			s_sprites->AddBackgroundSprite(background, Pimp::D3D::BlendMode::BM_None, -1, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));

			// background (or what used to be our loading screen)
//			s_sprites->AddSprite(
//					background,
//					Pimp::D3D::BlendMode::BM_Additive,
//					AlphaToVtxColor(1.f),
//					Vector2(0.f, 0.f), Vector2(1920.f, 1080.f),
//					1.f,
//					0.f);
		}
		else
		{
			s_sprites->AddBackgroundSprite(texWhite, Pimp::D3D::BlendMode::BM_None, 0, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));

			float backAlpha = (bitchBullets<1.f) ? bitchBullets : 1.f;

			// background
			s_sprites->AddSprite(
					background2,
					Pimp::D3D::BlendMode::BM_Additive,
					AlphaToVtxColor(backAlpha),
					Vector2(0.f, 0.f), Vector2(1920.f, 1080.f),
					1.f,
					0.f);

			// target sprite
			s_sprites->AddSpriteCenter(
					target,
					Pimp::D3D::BlendMode::BM_AlphaBlend,
					Vector2(1920.f*0.5f, 1080.f*0.5f),
					2.f,
					backAlpha,
					0.f);

			// pimp
			s_sprites->AddSpriteCenter(
				pimp,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(1920.f*0.5f, 1080.f*0.5f + 50.f), // little lower so his crotch covers the crosshair :)
				2.f,
				backAlpha,
				0.f);

			const float kShotZ = 2.5;
			const float ballY = 1080.f*0.5f-(blob->GetHeight()*0.5f);

			// gunshot holes
			int shotFX = (int)bitchBullets + 1;
			{
				static bool blasted[6];
				if (shotFX == 1) memset(blasted, 0, 6*sizeof(bool));

				if (shotFX > 1)
				{
					// the 1 offset is for the fading 'n such
					shotFX -=1;

					const Vector2 kShotPositions[5] = {
						Vector2(1920*0.5f + 135.f, ballY + 130.f),
						Vector2(1920*0.5f - 155.f, ballY - 50.f),
						Vector2(1920*0.5f - 180.f, ballY - 90.f),
						Vector2(1920*0.5f - 235.f, ballY + 160.f),
						Vector2(1920*0.5f + 200.f, ballY-70.f)
					};

					
					for (int iShot = 0; iShot < shotFX; ++iShot)
					{
						if (iShot < 5) {
							s_sprites->AddSprite(
									blast[iShot%3],
									Pimp::D3D::BlendMode::BM_AlphaBlend,
									kShotPositions[iShot],
									kShotZ,
									1.f,
									0.f);
						}

						// shotFX 5 means we play a sample only (shell drop)
						if (false == blasted[iShot])
						{
							if (iShot < 5)
								Audio_Gunshot();
							else
								Audio_Shelldrop();

							blasted[iShot] = true;
						}
					}
				}			
			}
		}
	}
};
