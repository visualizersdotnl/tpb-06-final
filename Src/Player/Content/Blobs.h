
#pragma once

static const unsigned int kNumMetaball4s = 14;
static __declspec(align(16)) Pimp::Metaballs::Metaball4 s_metaball4s[kNumMetaball4s];

class Blobs : public Scene
{
private:
	Pimp::Metaballs *pMetaballs;

public:
	Blobs()
	{
	}

	~Blobs()
	{
	}

	void ReqRocketTracks()
	{
	}

	void ReqAssets()
	{
	}

	void BindToWorld()
	{
		pMetaballs = new Pimp::Metaballs(gWorld);
		pMetaballs->Initialize();
		gWorld->GetElements().Add(pMetaballs);
		Pimp::World::StaticAddChildToParent(pMetaballs, gWorld->GetRootNode());
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();

		const float time = (float) sync_get_val(st_fxTime, row);

		for (unsigned int iBall4 = 0; iBall4 < kNumMetaball4s; ++iBall4)
		{
			for (unsigned int iBall = 0; iBall < 4; ++iBall)
			{
				const unsigned int ballCnt = iBall4*4 + iBall;
				s_metaball4s[iBall4].X[iBall] = 0.5f*sinf(ballCnt + time*1.1f);
				s_metaball4s[iBall4].Y[iBall] = 0.5f*sinf((ballCnt^7) + time*1.35f);
				s_metaball4s[iBall4].Z[iBall] = 0.5f*cosf((ballCnt^5) + time*1.4f);
			}
		}

		// Generate geometry (triggers visibility).
		pMetaballs->Generate(0.f, kNumMetaball4s, s_metaball4s, 190.f);
	}
};