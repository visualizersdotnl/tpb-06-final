
class Bondtro : public Demo::Scene
{
private:
	Pimp::Texture2D *background, *blob, *ampersand, *logoTPB, *logoInque, *present;
	Pimp::Texture2D *bigCircle, *target, *blast, *pimp, *title;

	const sync_track *st_bondBlob1;
	const sync_track *st_bondBlobFade1;
	const sync_track *st_bondBlob2;
	const sync_track *st_bondBlobFade2;
	const sync_track *st_bondAmpFade;
	const sync_track *st_bondFadeTPB;
	const sync_track *st_bondFadeINQ;
	const sync_track *st_bondPresFade;
	const sync_track *st_bondWhite;
	const sync_track *st_bondTarget;
	const sync_track *st_bondSoundFX;
	const sync_track *st_bondPimpFade;

public:
	Bondtro()
	{
	}

	~Bondtro()
	{
	}
	
	void ReqRocketTracks()
	{
		s_syncTracks.push_back(SyncTrack("bondBlob1", false, &st_bondBlob1));
		s_syncTracks.push_back(SyncTrack("bondBlobFade1", false, &st_bondBlobFade1));
		s_syncTracks.push_back(SyncTrack("bondBlob2", false, &st_bondBlob2));
		s_syncTracks.push_back(SyncTrack("bondBlobFade2", false, &st_bondBlobFade2));
		s_syncTracks.push_back(SyncTrack("bondAmpFade", false, &st_bondAmpFade));
		s_syncTracks.push_back(SyncTrack("bondFadeTPB", false, &st_bondFadeTPB));
		s_syncTracks.push_back(SyncTrack("bondFadeINQ", false, &st_bondFadeINQ));
		s_syncTracks.push_back(SyncTrack("bondPresFade", false, &st_bondPresFade));
		s_syncTracks.push_back(SyncTrack("bondWhite", false, &st_bondWhite));
		s_syncTracks.push_back(SyncTrack("bondTarget", false, &st_bondTarget));
		s_syncTracks.push_back(SyncTrack("bondSoundFX", false, &st_bondSoundFX));
		s_syncTracks.push_back(SyncTrack("bondPimpFade", false, &st_bondPimpFade));
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\bondtro\\bg.png", false, &background);
		Assets::AddTexture2D("textures\\bondtro\\bond-circle.png", false, &blob);
		Assets::AddTexture2D("textures\\bondtro\\bond-01.png", false, &ampersand);
		Assets::AddTexture2D("textures\\bondtro\\bond-00.png", false, &logoTPB);
		Assets::AddTexture2D("textures\\bondtro\\bond-02.png", false, &logoInque);
		Assets::AddTexture2D("textures\\bondtro\\bond-03.png", false, &present);
		Assets::AddTexture2D("textures\\bondtro\\bond-bullseye.png", false, &target);
		Assets::AddTexture2D("textures\\bondtro\\bond-hole.png", false, &blast);
		Assets::AddTexture2D("textures\\bondtro\\bond-meneer.png", false, &pimp);
		Assets::AddTexture2D("textures\\bondtro\\bond-title.png", false, &title);

//		Assets::AddTexture2D("textures\\1\\bond-circle.png", false, &bigCircle);
		bigCircle = nullptr;
	}

	void BindToWorld()
	{
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();
		s_sprites->AddBackgroundSprite(Pimp::gD3D->GetWhiteTex(), Pimp::D3D::BlendMode::BM_None, 0, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));

		const float ballPos_1 = (float) sync_get_val(st_bondBlob1, row);
		const float ballFade_1 = (float) sync_get_val(st_bondBlobFade1, row);
		const float ballPos_2 = (float) sync_get_val(st_bondBlob2, row);
		const float ballFade_2 = (float) sync_get_val(st_bondBlobFade2, row);
		const float ampFade = (float) sync_get_val(st_bondAmpFade, row);
		const float fadeTPB = (float) sync_get_val(st_bondFadeTPB, row);
		const float fadeINQ = (float) sync_get_val(st_bondFadeINQ, row);
		const float presentFade = (float) sync_get_val(st_bondPresFade, row);
		const float whiteOpacity = (float) sync_get_val(st_bondWhite, row);
		const float targetOpacity = (float) sync_get_val(st_bondTarget, row);
		const int   shotFX = (int) sync_get_val(st_bondSoundFX, row);
		const float pimpOpacity = (float) sync_get_val(st_bondPimpFade, row);

		const float ballY = 1080.f*0.5f-(blob->GetHeight()*0.5f);

		const float kTextZ = 1.f;
		const float kAmpersandZ = 1.1f;
		const float kBackgroundZ = 2.f;
		const float kTargetZ = 3.f;
		const float kShotZ = 4.f;
		const float kPimpZ = 5.f;
		const float kTitleZ = 6.f;

		// ball #1
		s_sprites->AddSprite(
				blob,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(ballPos_1, ballY),
				kTextZ,
				1.f-ballFade_1,
				0.f);

		// ball #2
		s_sprites->AddSprite(
				blob,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(ballPos_2, ballY),
				kTextZ,
				1.f-ballFade_2,
				0.f);

		// ampersand
		s_sprites->AddSprite(
				ampersand,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(ballPos_2, ballY),
				kAmpersandZ,
				1.f-ampFade,
				0.f);

		// group names
		s_sprites->AddSprite(
				logoTPB,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(ballPos_2-690.f-25.f, ballY),
				kTextZ,
				fadeTPB,
				0.f);
		s_sprites->AddSprite(
				logoInque,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(ballPos_2+160.f-20.f, ballY+5.f), // @plek: De een is net wat hoger dan de ander?
				kTextZ,
				fadeINQ,
				0.f);

		// "present.."
		s_sprites->AddSprite(
				present,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(ballPos_2+160.f+260.f, ballY+5.f), // @plek: And this one is off too :)
				kTextZ,
				1.f-presentFade,
				0.f);

		// background
		s_sprites->AddSprite(
				background,
				Pimp::D3D::BlendMode::BM_Additive,
				AlphaToVtxColor(whiteOpacity),
				Vector2(0.f, 0.f), Vector2(1920.f, 1080.f),
				kBackgroundZ,
				0.f);

#if 0
		// big circle (FIXME: to scale up in transition)
		s_sprites->AddSpriteCenter(
				bigCircle,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(1920.f*0.5f, 1080.f*0.5f),
				kTargetZ,
				0.f);
#endif

		// target sprite
		s_sprites->AddSpriteCenter(
				target,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(1920.f*0.5f, 1080.f*0.5f),
				kTargetZ+0.1f,
				targetOpacity,
				0.f);

		// gunshot holes
		{
			static bool blasted[4];
			if (shotFX == 0) memset(blasted, 0, 4*sizeof(bool));

			if (shotFX != 0)
			{

				const Vector2 kShotPositions[3] = {
					Vector2(1920*0.5f - 190.f, ballY - 135.f),
					Vector2(1920*0.5f + 135.f, ballY + 130.f),
					Vector2(1920*0.5f + 190.f, ballY) };

				for (int iShot = 0; iShot < shotFX; ++iShot)
				{
					if (iShot < 3) {
						s_sprites->AddSprite(
								blast,
								Pimp::D3D::BlendMode::BM_Subtractive,
								kShotPositions[iShot],
								kShotZ,
								1.f,
								0.f);
					}

					// shotFX 4 means we play a sample only (shell drop)
					if (false == blasted[iShot])
					{
						if (iShot < 3)
							Audio_Gunshot();
						else
							Audio_Shelldrop();

						blasted[iShot] = true;
					}
				}
			}			
		}

		// pimp
		s_sprites->AddSpriteCenter(
			pimp,
			Pimp::D3D::BlendMode::BM_AlphaBlend,
			Vector2(1920.f*0.5f, 1080.f*0.5f + 50.f), // little lower so his crotch covers the crosshair :)
			kPimpZ,
			pimpOpacity,
			0.f);

		// title
#if 0
		s_sprites->AddSpriteCenter(
			title,
			Pimp::D3D::BlendMode::BM_Subtractive,
			Vector2(1920.f*0.5f, 1080.f*0.5f),
			kTitleZ,
			1.f,
			0.f);
#endif
	}
};
