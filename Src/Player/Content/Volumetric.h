
class Volumetric : public Scene
{
private:
//	Pimp::Texture2D *texWall, *texMesh;
	Pimp::Material *shaftMat;

public:
	Volumetric()
	{
	}

	~Volumetric()
	{
	}

	void ReqAssets()
	{
		Assets::AddMaterial("shaders\\Scene_Ribbons.fx", &shaftMat);
	}

	void BindAnimationNodes()
	{
	}

	void BindAssets()
	{
		BindSceneMaterial(shaftMat);
	}

	void Tick(double row)
	{
		SetSceneMaterial();
		SetMainSceneAndDefaultCamera();
	}
};
