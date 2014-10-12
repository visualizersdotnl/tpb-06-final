
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
		//Assets::AddTexture2D("textures\\ribbons_wall.png", true, &texWall);
		//Assets::AddTexture2D("textures\\ribbons_mesh.png", true, &texMesh);
		//Assets::AddTexture2D("textures\\even_lachen.png", true, &texMesh);
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
