
#pragma once

#include "D3D.h"
#include "Effect.h"
#include "EffectPass.h"
#include "EffectTechnique.h"
#include "Camera.h"
#include "Xform.h"

namespace Pimp 
{
	class Metaballs
	{
	public:
		struct Metaball4
		{
			float X[4];
			float Y[4];
			float Z[4];
		};

		Metaballs();
		~Metaballs();

		bool Initialize();
		void Generate(unsigned int numBall4s, const Metaball4 *pBall4s, float surfaceLevel);
		void Draw(Camera* camera);
		
		void SetRotation(const Quaternion &rotation);
		void SetMaps(Texture2D *envMap, Texture2D *projMap, float projScrollU, float projScrollV);
		void SetLighting(float shininess, float overbright);
		void SetRim(float rim);

	private:
		ID3D10Buffer *pVB, *pIB;
		ID3D10InputLayout *inputLayout;

		Effect effect;
		EffectTechnique effectTechnique;
		EffectPass effectPass;

		int varIndexTextureMap;
		int varIndexProjMap;
		int varIndexViewProjMatrix;
		int varIndexWorldMatrix;
		int varIndexWorldMatrixInv;
		int varIndexShininess;
		int varIndexOverbright;
		int varIndexProjScroll;
		int varIndexRim;

		bool isVisible;

		Xform *worldTrans;
		float shininess;
		float overbright;

		Texture2D *envMap, *projMap;
		float projScrollU, projScrollV;
		float rim;

		__forceinline unsigned int GetEdgeTableIndex();
		float CalculateIsoValue(unsigned int iGrid, float gridX, float gridY, float gridZ);
		void ProcessCube(unsigned int iGrid, unsigned int iX, unsigned int iY, unsigned int iZ);
		void Triangulate(unsigned int iGrid, float gridX, float gridY, float gridZ, unsigned int iEdgeTab, unsigned int edgeBits);
	};
}