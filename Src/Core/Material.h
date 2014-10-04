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

		int varIndexScaleFactor;			// scale factor to render with (XY). used for aspect ratio correction.
		int varIndexViewInvMatrix;			// inv-view matrix
		int varIndexOverlayOpacity;			// overlay opacity
		int varIndexOverlayProgress;		// progress of the overlay (0..1)
		int varIndexSceneRenderLOD;			// scene render LOD. 1 = full screen, 0.5 = half size, 0.25 = quarter of screen, etc...
		int varIndexSceneBuffer;			// scene buffer texture (only allowed in post effect material)


#ifdef _DEBUG
		std::string shaderFileName; // complete path to shader source file
#endif

	protected:
		virtual void InitParameters();

	public:
		Material(
			World* world, 
			const unsigned char* shaderCompiledText, int shaderCompiledTextLength, 
			const std::string& shaderFileName);

		~Material();

		void Bind(Camera* camera);

#ifdef _DEBUG
		const std::string& GetShaderFileName() const
		{
			return shaderFileName;
		}
#endif

		void RefreshParameters();

		void SetOverlayOpacityAndProgress(float opacity, float progress, float timeSoFar);
		void SetSceneBuffer(ID3D10ShaderResourceView* resourceView);
	};
}