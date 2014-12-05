
#pragma once

#include <Math/math.h>
// #include <Effects11/inc/d3dx11effect.h>

namespace Pimp
{
	class Effect : public boost::noncopyable
	{
	public:
		Effect(const void* compiledEffect, int compiledEffectLength);
		~Effect();

		ID3DX11EffectTechnique* GetTechnique(const char* name) const;

		// Register a shader variable and return its index
		int RegisterVariable(const char* name, bool required);

		// See if a shader variable is available
		bool HasVariable(const char* name) const;

		void SetVariableValue(int index, float value);
		void SetVariableValue(int index, const Matrix4& value);
		void SetVariableValue(int index, const Vector2& value);
		void SetVariableValue(int index, const Vector4& value);
		void SetVariableValue(int index, ID3D11ShaderResourceView* shaderResource);
		void SetVariableValue(int index, Vector4* value, int numValues);
		void SetVariableValue(int index, float* value, int numValues);

		void ResetRegisteredVariables();

#ifdef _DEBUG
		const D3DX11_EFFECT_TYPE_DESC GetTypeDesc(int index) const;
#endif

	private:
		ID3DX11Effect* effect;
		std::vector<ID3DX11EffectVariable*> variables;
	};
}
