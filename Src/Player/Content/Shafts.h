
class Shafts : public Scene
{
private:
	Pimp::Material *shaftMat;
	Pimp::MaterialParameter *rotation0Param;
	Pimp::MaterialParameter *rotation1Param;
	Pimp::MaterialParameter *rotation2Param;

	Pimp::Texture2D *vignette;

	const sync_track *st_shaftsRot0x;
	const sync_track *st_shaftsRot0y;
	const sync_track *st_shaftsRot0z;
	const sync_track *st_shaftsRot1x;
	const sync_track *st_shaftsRot1y;
	const sync_track *st_shaftsRot1z;
	const sync_track *st_shaftsRot2x;
	const sync_track *st_shaftsRot2y;
	const sync_track *st_shaftsRot2z;
	
public:
	Shafts()
	{
	}

	~Shafts()
	{
	}

	void ReqRocketTracks()
	{
		s_syncTracks.push_back(SyncTrack("shaftsRot0x", false, &st_shaftsRot0x));
		s_syncTracks.push_back(SyncTrack("shaftsRot0y", false, &st_shaftsRot0y));
		s_syncTracks.push_back(SyncTrack("shaftsRot0z", false, &st_shaftsRot0z));
		s_syncTracks.push_back(SyncTrack("shaftsRot1x", false, &st_shaftsRot1x));
		s_syncTracks.push_back(SyncTrack("shaftsRot1y", false, &st_shaftsRot1y));
		s_syncTracks.push_back(SyncTrack("shaftsRot1z", false, &st_shaftsRot1z));
		s_syncTracks.push_back(SyncTrack("shaftsRot2x", false, &st_shaftsRot2x));
		s_syncTracks.push_back(SyncTrack("shaftsRot2y", false, &st_shaftsRot2y));
		s_syncTracks.push_back(SyncTrack("shaftsRot2z", false, &st_shaftsRot2z));
		s_syncTracks.push_back(SyncTrack("shaftsLightAmount", true));	
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\shafts\\shafts_noise.png", NULL);
		Assets::AddTexture2D("textures\\shafts\\shafts_rock.png", NULL);
		Assets::AddTexture2D("textures\\shafts\\shafts_room.png", NULL);
		Assets::AddTexture2D("textures\\shafts\\vignette.png", &vignette);

		Assets::AddMaterial("shaders\\Scene_Shafts.fx", &shaftMat);
	}

	void BindToWorld()
	{
		BindSceneMaterial(shaftMat);		
		
		rotation0Param = AddMaterialParamWithXform("testBallXformInv0", true);
		rotation1Param = AddMaterialParamWithXform("testBallXformInv1", true);
		rotation2Param = AddMaterialParamWithXform("testBallXformInv2", true);
	}

	void Tick(double row)
	{
		const float rotation0x = (float) sync_get_val(st_shaftsRot0x, row);
		const float rotation0y = (float) sync_get_val(st_shaftsRot0y, row);
		const float rotation0z = (float) sync_get_val(st_shaftsRot0z, row);
		const float rotation1x = (float) sync_get_val(st_shaftsRot1x, row);
		const float rotation1y = (float) sync_get_val(st_shaftsRot1y, row);
		const float rotation1z = (float) sync_get_val(st_shaftsRot1z, row);
		const float rotation2x = (float) sync_get_val(st_shaftsRot2x, row);
		const float rotation2y = (float) sync_get_val(st_shaftsRot2y, row);
		const float rotation2z = (float) sync_get_val(st_shaftsRot2z, row);

		Quaternion rotation0 = CreateQuaternionFromYawPitchRoll(rotation0x, rotation0y, rotation0z);
		Quaternion rotation1 = CreateQuaternionFromYawPitchRoll(rotation1x, rotation1y, rotation1z);
		Quaternion rotation2 = CreateQuaternionFromYawPitchRoll(rotation2x, rotation2y, rotation2z);

		((Pimp::Xform*)rotation0Param->GetParents()[0])->SetRotation(rotation0);
		((Pimp::Xform*)rotation1Param->GetParents()[0])->SetRotation(rotation1);
		((Pimp::Xform*)rotation2Param->GetParents()[0])->SetRotation(rotation2);

		SetMainSceneAndDefaultCamera();
		s_sprites->AddBackgroundSprite(texWhite, Pimp::D3D::BlendMode::BM_None, 0, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));

		// vignette (hides artifacts on the corners of the background box)
		s_sprites->AddSpriteCenter(vignette, Pimp::D3D::BlendMode::BM_AlphaMask, Vector2(1920.f*0.5f, 1080.f*0.5f), 1.f, 1.f, 0.f);
	}
};
