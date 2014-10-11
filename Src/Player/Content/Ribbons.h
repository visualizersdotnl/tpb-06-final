
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
		Assets::AddTexture2D("textures\\ribbons_wall.png", true, &texWall);
		Assets::AddTexture2D("textures\\ribbons_mesh.png", true, &texMesh);
		Assets::AddTexture2D("textures\\even_lachen.png", true, &texMesh);
		Assets::AddMaterial("shaders\\Scene_Ribbons.fx", &ribbonMat);
	}

	void BindAnimationNodes()
	{
	}

	void BindAssets()
	{
		BindSceneMaterial(ribbonMat);
	}

	void Tick()
	{
		SetSceneMaterial();
		SetMainSceneAndDefaultCamera();
	}
};
