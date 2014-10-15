
class Bondtro : public Demo::Scene
{
private:
	Pimp::Texture2D *bondBlob, *ampersand, *logoTPB, *logoInque, *present;
	Pimp::Texture2D *bigCircle, *target, *blast, *pimp, *title;

public:
	Bondtro()
	{
	}

	~Bondtro()
	{
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\1\\bond-circle.png", false, &bondBlob); // 106*106
		Assets::AddTexture2D("textures\\1\\bond-01.png", false, &ampersand); // 106*106
		Assets::AddTexture2D("textures\\1\\bond-00.png", false, &logoTPB);
		Assets::AddTexture2D("textures\\1\\bond-02.png", false, &logoInque);
		Assets::AddTexture2D("textures\\1\\bond-03.png", false, &present);
		Assets::AddTexture2D("textures\\1\\bond-circle.png", false, &bigCircle);
		Assets::AddTexture2D("textures\\1\\bond-bullseye.png", false, &target);
		Assets::AddTexture2D("textures\\1\\bond-hole.png", false, &blast);
		Assets::AddTexture2D("textures\\1\\bond-meneer.png", false, &pimp);
		Assets::AddTexture2D("textures\\1\\bond-title.png", false, &title);
	}

	void BindAnimationNodes()
	{
	}

	void BindAssets()
	{
		SetSceneMaterial();
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();

		const float ballPos_1 = (float) sync_get_val(st_bondBlob1, row);
		const float ballFade_1 = (float) sync_get_val(st_bondBlobFade1, row);
		const float ballPos_2 = (float) sync_get_val(st_bondBlob2, row);
		const float ballFade_2 = (float) sync_get_val(st_bondBlobFade2, row);
		const float ampFade = (float) sync_get_val(st_bondAmpFade, row);
		const float groupsFade = (float) sync_get_val(st_bondGroupsFade, row);
		const float presentFade = (float) sync_get_val(st_bondPresFade, row);
		const float whiteOpacity = (float) sync_get_val(st_bondWhite, row);
		const float targetOpacity = (float) sync_get_val(st_bondTarget, row);
		const int   shotFX = (int) sync_get_val(st_bondSoundFX, row);
		const float pimpOpacity = (float) sync_get_val(st_bondPimpFade, row);

		const float ballY = 1080.f*0.5f-(bondBlob->GetHeight()*0.5f);

		const float kTextZ = 1.f;
		const float kBackgroundZ = 2.f;
		const float kTargetZ = 3.f;
		const float kShotZ = 4.f;
		const float kPimpZ = 5.f;
		const float kTitleZ = 6.f;

		// ball #1
		s_sprites->AddSprite(
				bondBlob,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(ballPos_1, ballY),
				kTextZ,
				1.f-ballFade_1,
				0.f);

		// ball #2
		s_sprites->AddSprite(
				bondBlob,
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
				kTextZ,
				1.f-ampFade,
				0.f);

		// group names
		s_sprites->AddSprite(
				logoTPB,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(ballPos_2-690.f-25.f, ballY),
				kTextZ,
				1.f-groupsFade,
				0.f);
		s_sprites->AddSprite(
				logoInque,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(ballPos_2+160.f-20.f, ballY+5.f), // @plek: De een is net wat hoger dan de ander?
				kTextZ,
				1.f-groupsFade,
				0.f);

		// "present.."
		s_sprites->AddSprite(
				present,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(ballPos_2+160.f+260.f, ballY+5.f), // @plek: And this one is off too :)
				kTextZ,
				1.f-presentFade,
				0.f);

		// (white) background
		s_sprites->AddSprite(
				texWhite,
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
					Vector2(1920*0.5f - 100.f, ballY - 50.f),
					Vector2(1920*0.5f + 135.f, ballY + 130.f),
					Vector2(1920*0.5f + 200.f, ballY) };

				for (int iShot = 0; iShot < shotFX; ++iShot)
				{
					if (iShot < 3) {
						s_sprites->AddSprite(
								blast,
								Pimp::D3D::BlendMode::BM_Additive,
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
