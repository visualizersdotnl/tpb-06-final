
class Bondtro : public Demo::Scene
{
private:
	Pimp::Texture2D *bondBlob, *ampersand, *logoTPB, *logoInque, *present;

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
		Assets::AddTexture2D("textures\\1\\bond-01.png", true, &ampersand); // 106*106
		Assets::AddTexture2D("textures\\1\\bond-00.png", true, &logoTPB);
		Assets::AddTexture2D("textures\\1\\bond-02.png", true, &logoInque);
		Assets::AddTexture2D("textures\\1\\bond-03.png", true, &present);
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

		const float ballPos_1 = (float) sync_get_val(st_bondBlob1, row);
		const float ballFade_1 = (float) sync_get_val(st_bondBlobFade1, row);
		const float ballPos_2 = (float) sync_get_val(st_bondBlob2, row);
		const float ballFade_2 = (float) sync_get_val(st_bondBlobFade2, row);
		const float ampFade = (float) sync_get_val(st_bondAmpFade, row);
		const float groupsFade = (float) sync_get_val(st_bondGroupsFade, row);
		const float presentFade = (float) sync_get_val(st_bondPresFade, row);

		const float ballY = 1080.f*0.5f-(bondBlob->GetHeight()*0.5f);
	
		// ball #1
		s_sprites->AddSprite(
				bondBlob,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(ballPos_1, ballY),
				0.f,
				1.f-ballFade_1);

		// ball #2
		s_sprites->AddSprite(
				bondBlob,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(ballPos_2, ballY),
				0.f,
				1.f-ballFade_2);

		// ampersand
		s_sprites->AddSprite(
				ampersand,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(ballPos_2, ballY),
				0.f,
				1.f-ampFade);

		// group names
		s_sprites->AddSprite(
				logoTPB,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(ballPos_2-690.f-25.f, ballY),
				0.f,

				1.f-groupsFade);
		s_sprites->AddSprite(
				logoInque,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(ballPos_2+160.f-20.f, ballY+5.f), // @plek: De een is net wat hoger dan de ander?
				0.f,
				1.f-groupsFade);

		// "present.."
		s_sprites->AddSprite(
				present,
				Pimp::D3D::BlendMode::BM_AlphaBlend,
				Vector2(ballPos_2+160.f+260.f, ballY+5.f), // @plek: And this one is off too :)
				0.f,
				1.f-presentFade);

	}
};
