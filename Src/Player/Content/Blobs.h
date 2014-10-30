
#pragma once

static const unsigned int kNumMetaball4s = 2;
static __declspec(align(16)) Pimp::Metaballs::Metaball4 s_metaball4s[kNumMetaball4s];

class Blobs : public Scene
{
private:
	Pimp::Metaballs *pMetaballs;

public:
	Blobs()
	{
		pMetaballs = new Pimp::Metaballs(gWorld);
		pMetaballs->Initialize();
		gWorld->GetElements().Add(pMetaballs);
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

	void BindAnimationNodes()
	{
	}

	void BindAssets()
	{
	}

	void Tick(double row)
	{
		SetMainSceneAndDefaultCamera();

		// Always visible.
		pMetaballs->SetVisible(true);

		float blobT = 0.f;
		for (unsigned int iBall4 = 0; iBall4 < kNumMetaball4s; ++iBall4)
		{
			for (unsigned int iBall = 0; iBall < 4; ++iBall)
			{
				const unsigned int ballCnt = iBall4*4 + iBall;
				s_metaball4s[iBall4].X[iBall] = 0.6f*sinf(ballCnt + blobT*1.29f);
				s_metaball4s[iBall4].Y[iBall] = 0.5f*sinf((ballCnt^8) + blobT*2.75f);
				s_metaball4s[iBall4].Z[iBall] = 0.5f*cosf((ballCnt^5) + blobT*1.25f);
			}
		}

		// Tick manually.
		pMetaballs->Tick(0.f, kNumMetaball4s, s_metaball4s, 21.f);
	}
};
