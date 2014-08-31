#pragma once

#include "Effect.h"
#include "EffectTechnique.h"
#include "EffectPass.h"
#include "Camera.h"
#include "MaterialParameter.h"
#include "Balls.h"

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
		char shaderName[256]; // name of the shader source file, to easily determine changes made in maya
		unsigned int shaderCRC; // CRC of the shader text, to easily determine changes made in maya
#endif

	protected:
		virtual void InitParameters();

	public:
		Material(
			World* world, 
			const unsigned char* shaderCompiledText, int shaderCompiledTextLength, 
			const char* shaderName, unsigned int shaderCRC);

#ifdef _DEBUG
		~Material();
#endif

		void Bind(Camera* camera);

#ifdef _DEBUG
		unsigned int GetShaderCRC() const 
		{ 
			return shaderCRC; 
		}

		const char* GetShaderName() const
		{
			return shaderName;
		}
#endif

		void RefreshParameters();

		void SetOverlayOpacityAndProgress(float opacity, float progress, float timeSoFar);
		void SetSceneBuffer(ID3D10ShaderResourceView* resourceView);
	};
}