
class Pompom : public Scene
{
private:
	Pimp::Material *sceneMat;
	Pimp::Texture2D *background;
	Pimp::MaterialParameter *rotParam;
	Pimp::MaterialParameter *rotParamInv;

public:
	Pompom()
	{
	}

	~Pompom()
	{
	}

	void ReqRocketTracks()
	{
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\pompom\\background.png", &background);		
		Assets::AddTexture2D("textures\\pompom\\pompom_noise.png", NULL);
		Assets::AddTexture2D("textures\\pompom\\pompom_color.png", NULL);
		Assets::AddMaterial("shaders\\Scene_Pompom.fx", &sceneMat);
	}

	void BindToWorld()
	{
		BindSceneMaterial(sceneMat);
		sceneMat->SetBlendMode(Pimp::D3D::BlendMode::BM_AlphaBlend);
		rotParam = AddMaterialParamWithXform("pompomRotMat", false);
//		rotParamInv = AddMaterialParamWithXform("pompomRotMatInv", true);
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();
		s_sprites->AddBackgroundSprite(0, background, Pimp::D3D::BlendMode::BM_None, -1, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));
		s_sprites->SkipBGSprite2(true);

		// fxtimer
		float time = (float) sync_get_val(st_fxTime, row);

		// rotatie
		float yaw = time*0.2f;
		float pitch = time*0.3f;
		float roll = time*0.1f;
		Quaternion rotation = CreateQuaternionFromYawPitchRoll(yaw, pitch, roll);
		((Pimp::Xform*)rotParam->GetParents()[0])->SetRotation(rotation);
//		((Pimp::Xform*)rotParamInv->GetParents()[0])->SetRotation(rotation);
	}
};
