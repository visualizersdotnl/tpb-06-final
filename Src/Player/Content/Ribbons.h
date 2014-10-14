
class Ribbons : public Scene
{
private:
	Pimp::Texture2D *texWall, *texMesh;
	Pimp::Material *ribbonMat;

public:
	Ribbons()
	{
	}

	~Ribbons()
	{
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\2\\even_lachen.png", true, &texMesh);
		Assets::AddMaterial("shaders\\Scene_Ribbons.fx", &ribbonMat);
	}

	void BindAnimationNodes()
	{
	}

	void BindAssets()
	{
		BindSceneMaterial(ribbonMat);
	}

	void Tick(double row)
	{
		SetSceneMaterial();
		SetMainSceneAndDefaultCamera();
	}
};
