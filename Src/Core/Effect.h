#pragma once

#include "D3D.h"
#include <Shared/shared.h>

namespace Pimp
{
	class Effect
	{
	public:
		Effect(const unsigned char* compiledEffect, int compiledEffectLength);
		~Effect();

		ID3D10EffectTechnique* GetTechnique(const char* name);

		// Register a shader variable and return its index
		int RegisterVariable(const char* name, bool required);

		// See if a shader variable is available
		bool HasVariable(const char* name) const;

		void SetVariableValue(int index, float value);
		void SetVariableValue(int index, const Matrix4& value);
		void SetVariableValue(int index, const Vector2& value);
		void SetVariableValue(int index, const Vector4& value);
		void SetVariableValue(int index, ID3D10ShaderResourceView* shaderResource);
		void SetVariableValue(int index, Vector4* value, int numValues);
		void SetVariableValue(int index, float* value, int numValues);

		void ResetRegisteredVariables();

#ifdef _DEBUG
		D3D10_EFFECT_TYPE_DESC GetTypeDesc(int index) const;
#endif

	private:
		ID3D10Effect* effect;
		FixedSizeList<ID3D10EffectVariable*> variables;
	};
}

