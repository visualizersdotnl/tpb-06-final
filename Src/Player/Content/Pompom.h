
class Pompom : public Scene
{
private:
	Pimp::Material *sceneMat;
	Pimp::Texture2D *background;
	Pimp::MaterialParameter *rotParam;
	Pimp::MaterialParameter *rotParamInv;

	const sync_track *st_furStretchY;

public:
	Pompom()
	{
	}

	~Pompom()
	{
	}

	void ReqRocketTracks()
	{
		s_syncTracks.push_back(SyncTrack("furBackStretchY", false, &st_furStretchY));
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
		sceneMat->SetBlendMode(Pimp::D3D::Blend::BM_AlphaBlend);
		rotParam = AddMaterialParamWithXform("pompomRotMat", false);
//		rotParamInv = AddMaterialParamWithXform("pompomRotMatInv", true);
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();
//		s_sprites->AddBackgroundSprite(background, Pimp::D3D::Blend::BM_None, -1, 1.f, true);
		float stretchY = (float) sync_get_val(st_furStretchY, row);
		s_sprites->AddBackgroundSprite(
			background,
			Pimp::D3D::Blend::BM_None,
			-1,
			Vector2(0.f, -stretchY),
			Vector2(1920.f, 1080.f+stretchY),
			1.f,
			0.f,
			true);

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
