
class Volumetric : public Scene
{
private:
//	Pimp::Texture2D *texWall, *texMesh;
	Pimp::Material *shaftMat;
	Pimp::MaterialParameter *rotation0Param;
	Pimp::MaterialParameter *rotation1Param;
	Pimp::MaterialParameter *rotation2Param;

	Pimp::MaterialParameter* AddMaterialParam(const char* name)
	{
		Pimp::Xform* xform = new Pimp::Xform(gWorld);
		
		Pimp::MaterialParameter* param = new Pimp::MaterialParameter(gWorld);
		gWorld->GetElements().Add(param);
		param->SetValueType(Pimp::MaterialParameter::VT_NodeXformInv);
		param->SetName(name);

		Pimp::World::StaticAddChildToParent(xform, gWorld->GetRootNode());
		Pimp::World::StaticAddChildToParent(param, xform);

		return param;
	}

public:
	Volumetric()
	{
	}

	~Volumetric()
	{
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\6\\shafts_noise.png", true, NULL);
		Assets::AddTexture2D("textures\\6\\shafts_rock.png", true, NULL);
		Assets::AddTexture2D("textures\\6\\shafts_room.png", true, NULL);

		Assets::AddMaterial("shaders\\Scene_Shafts.fx", &shaftMat);
	}

	void BindAnimationNodes()
	{
	}

	void BindAssets()
	{
		BindSceneMaterial(shaftMat);		
		
		rotation0Param = AddMaterialParam("testBallXformInv0");
		rotation1Param = AddMaterialParam("testBallXformInv1");
		rotation2Param = AddMaterialParam("testBallXformInv2");
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

		SetSceneMaterial();
		SetMainSceneAndDefaultCamera();
	}
};
