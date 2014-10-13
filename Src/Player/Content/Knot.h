
class Knot : public Scene
{
private:
	Pimp::Texture2D *texWall, *texMesh;
	Pimp::Material *sceneMat;

public:
	Knot()
	{
	}

	~Knot()
	{
	}

	void ReqAssets()
	{
		Assets::AddMaterial("shaders\\Scene_Knot.fx", &sceneMat);
	}

	void BindAnimationNodes()
	{
	}

	void BindAssets()
	{
		BindSceneMaterial(sceneMat);
	}

	void Tick(double row)
	{
		SetSceneMaterial();
		SetMainSceneAndDefaultCamera();
	}
};
