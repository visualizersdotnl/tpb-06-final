
#pragma once

#include "Effect.h"
#include "EffectTechnique.h"
#include "EffectPass.h"
#include "Camera.h"
#include "MaterialParameter.h"

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

		std::vector<int> boundTextureIndices;
		std::vector<BoundMaterialParameter> boundMaterialParameters;

		int varIndexViewInvMatrix;  // Inverse view matrix.
		int varIndexSceneRenderLOD;	// Scene render LOD: 1 = full, 0.5 = half size, 0.25 = quarter, et cetera.
		int varIndexSceneBuffer;    // Scene buffer texture/RT (only allowed in post effect material).

		D3D::Blend blendMode;

		// Complete path to shader source file
		const std::string shaderFileName; 
protected:
		virtual void InitParameters();

	public:
		Material(World* world, const void* compiledEffect, int compiledEffectLength, const std::string& shaderFileName);
		~Material();

		void Bind(Camera* camera);

		void RefreshParameters();
		void SetSceneBuffer(ID3D11ShaderResourceView* resourceView);

		void SetBlendMode(D3D::Blend mode) { blendMode = mode; }
		D3D::Blend GetBlendMode() const    { return blendMode; }

		// Exposed to verify vertex shader input(s).
		const EffectPass& GetEffectPass() const 
		{ 
			return effectPass; 
		}

		const std::string& GetShaderFileName() const
		{
			return shaderFileName;
		}
	};
}
