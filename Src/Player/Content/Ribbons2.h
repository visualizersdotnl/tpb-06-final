
class Ribbons2 : public Scene
{
private:
	Pimp::Texture2D *texWall, *texMesh;
	Pimp::Material *ribbonMat;

	const sync_track *st_toBG2;

public:
	Ribbons2()
	{
	}

	~Ribbons2()
	{
	}

	void ReqRocketTracks()
	{
		s_syncTracks.push_back(SyncTrack("ribbonsAppear", true));	
		s_syncTracks.push_back(SyncTrack("ribbonsPhase", true));	
		s_syncTracks.push_back(SyncTrack("ribbonsWonkyness", true));		
		s_syncTracks.push_back(SyncTrack("ribbons2Separate", true));		

		s_syncTracks.push_back(SyncTrack("ribbons2ToBG2", false, &st_toBG2));		
	}

	void ReqAssets()
	{
		Assets::AddTexture2D("textures\\ribbons2\\background.png", &texWall);
		Assets::AddTexture2D("textures\\ribbons2\\even_lachen.png", &texMesh);
		Assets::AddMaterial("shaders\\Scene_Ribbons2.fx", &ribbonMat);
	}

	void BindToWorld()
	{
		BindSceneMaterial(ribbonMat);
		ribbonMat->SetBlendMode(Pimp::D3D::BlendMode::BM_AlphaBlend);
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();
		s_sprites->AddBackgroundSprite(0, texWall, Pimp::D3D::BlendMode::BM_None, -1, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));

		float toBG2 = (float) sync_get_val(st_toBG2, row);
		if (toBG2 == 0.f)
		{
			s_sprites->SkipBGSprite2(true);
		}
		else
		{
			s_sprites->SkipBGSprite2(false);

			unsigned int vtxCol1 = AlphaToVtxColor(1.f-toBG2);
			unsigned int vtxCol2 = AlphaToVtxColor(toBG2);

			s_sprites->AddBackgroundSprite(0, texWall, Pimp::D3D::BlendMode::BM_Additive, vtxCol1, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));
			s_sprites->AddBackgroundSprite(1, texWhite, Pimp::D3D::BlendMode::BM_Additive, vtxCol2, Vector2(0.f, 0.f), Vector2(1920.f, 1080.f), Vector2(1.f, 1.f));
		}
	}
};
