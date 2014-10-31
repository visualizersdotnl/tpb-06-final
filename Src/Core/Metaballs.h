
#pragma once

#include "D3D.h"
//#include "Node.h"
#include "Effect.h"
#include "EffectPass.h"
#include "EffectTechnique.h"
#include "Camera.h"
#include "Xform.h"

namespace Pimp 
{
	class Metaballs : public Node
	{
	protected:

	public:
		struct Metaball4
		{
			float X[4];
			float Y[4];
			float Z[4];
		};

		Metaballs(World* ownerWorld);
		virtual ~Metaballs();
		
		virtual void Tick(float deltaTime);
		virtual bool HasGeometry() const { return true; }

		bool ShouldRender() const 
		{
			return isVisible;
		}

		bool Initialize();
		void Generate(float deltaTime, unsigned int numBall4s, const Metaball4 *pBall4s, float surfaceLevel);
		void Draw(Camera* camera);
		
		// FIXME: hack to rotate locally (via world matrix)
		void SetRotation(const Quaternion &rotation);

	private:
		ID3D10Buffer *m_pVB, *m_pIB;
		ID3D10InputLayout *m_inputLayout;

		Effect effect;
		EffectTechnique effectTechnique;
		EffectPass effectPass;

		int varIndexTextureMap;
		int varIndexViewProjMatrix;
		int varIndexWorldMatrix;
		int varIndexWorldMatrixInv;

		bool isVisible;

		// FIXME
		Xform *worldTrans; 

		__forceinline unsigned int GetEdgeTableIndex();
		float CalculateIsoValue(unsigned int iGrid, float gridX, float gridY, float gridZ);
		void ProcessCube(unsigned int iGrid, unsigned int iX, unsigned int iY, unsigned int iZ);
		void Triangulate(unsigned int iGrid, float gridX, float gridY, float gridZ, unsigned int iEdgeTab, unsigned int edgeBits);
	};
}