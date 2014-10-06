
class Ribbons : public Scene
{
private:
	Pimp::Texture2D *texWall, *texMesh;
	Pimp::Material *ribbonMat;
	Pimp::Sprite *testSprite;
	
	float testSpriteTime;

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
		Assets::AddMaterial("shaders\\Scene_Ribbons.fx", &ribbonMat);
	}

	void BindAnimationNodes()
	{
	}

	void BindAssets()
	{
		SetSceneMaterial(ribbonMat);

		testSprite = new Pimp::Sprite(Vector2(0,0), Vector2(0.1f, 0.1f), texWall);
		gWorld->GetSprites()->AddSprite(testSprite);
	}

	void Tick()
	{
		float t = gWorld->GetCurrentTime();
		Vector2 position = Vector2(0.5f * cos(t), 0.5f * sin(t*1.5f));

		testSprite->SetPosition(position);
	}
};
