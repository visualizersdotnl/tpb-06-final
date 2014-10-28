
#pragma once

#include "D3D.h"
#include "Node.h"
#include "Geometry.h"
#include "Effect.h"
#include "EffectPass.h"
#include "EffectTechnique.h"

namespace Pimp 
{
	class Metaballs : 
		public Geometry 
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

		virtual bool HasGeometry() const { return true; }
		virtual bool ShouldRender() const { return isVisible; }

		void Tick(float deltaTime, unsigned int numBall4s, const Metaball4 *pBall4s, float surfaceLevel);

		bool Initialize();
		void Draw();

	private:
		ID3D10Buffer *m_pVB, *m_pIB;
		ID3D10InputLayout *m_inputLayout;

		Effect effect;
		EffectTechnique effectTechnique;
		EffectPass effectPass;

		int varIndexTextureMap;
	};
}