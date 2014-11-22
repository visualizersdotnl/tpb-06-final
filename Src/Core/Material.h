#pragma once

#include "Effect.h"
#include "EffectTechnique.h"
#include "EffectPass.h"
#include "Camera.h"
#include "MaterialParameter.h"
#include "Balls.h"

#include <string>

namespace Pimp 
{
	class World;

	class Material
	{
	private:
		Effect effect;
		EffectTechnique effectTechnique;
		EffectPass effectPass;

		World* world;

		struct BoundMaterialParameter
		{
			int varIndex;
			MaterialParameter* parameter;
		};

		FixedSizeList<int> boundTextureVariableIndices;
		FixedSizeList<BoundMaterialParameter> boundMaterialParameters;

#ifdef _DEBUG
		Balls::BoundBalls boundBalls;
#endif

		int varIndexViewInvMatrix;			// inv-view matrix
		int varIndexSceneRenderLOD;			// scene render LOD. 1 = full screen, 0.5 = half size, 0.25 = quarter of screen, etc...
		int varIndexSceneBuffer;			// scene buffer texture (only allowed in post effect material)

		D3D::Blend blendMode;

		// complete path to shader source file
		std::string shaderFileName; 
protected:
		virtual void InitParameters();

	public:
		Material(
			World* world, 
			const unsigned char* shaderCompiledText, int shaderCompiledTextLength, 
			const std::string& shaderFileName);

		~Material();

		void Bind(Camera* camera);

		const std::string& GetShaderFileName() const
		{
			return shaderFileName;
		}

		void RefreshParameters();

		void SetSceneBuffer(ID3D10ShaderResourceView* resourceView);

		void SetBlendMode(D3D::Blend mode) { blendMode = mode; }
		D3D::Blend GetBlendMode() const { return blendMode; }
	};
}