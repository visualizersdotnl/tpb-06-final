
#include "Platform.h"
#include "D3D.h"
#include "Effect.h"

namespace Pimp
{
	Effect::Effect(const unsigned char* compiledEffect, int compiledEffectLength)
	{
		variables.reserve(16); // Reasonable pre-alloc.
		effect = gD3D->CreateEffect(compiledEffect, compiledEffectLength);
	}

	Effect::~Effect()
	{
		SAFE_RELEASE(effect);
	}

	ID3DX11EffectTechnique* Effect::GetTechnique(const char* name)
	{
		ID3DX11EffectTechnique* technique = effect->GetTechniqueByName(name);
		ASSERT(nullptr != technique);
		return technique->IsValid() ? technique : nullptr;
	}

	int Effect::RegisterVariable(const char* name, bool required)
	{
		ID3DX11EffectVariable* variable = effect->GetVariableByName(name);
	
		if (nullptr != variable && variable->IsValid())
		{
			variables.push_back(variable);
			return int(variables.size()-1);
		}
		else
		{
			ASSERT(false == required);
			return -1;
		}
	}

	bool Effect::HasVariable(const char* name) const
	{
		ID3DX11EffectVariable* variable = effect->GetVariableByName(name);
		return nullptr != variable && variable->IsValid();
	}

	void Effect::ResetRegisteredVariables()
	{
		variables.clear();
	}

#ifdef _DEBUG

	const D3DX11_EFFECT_TYPE_DESC Effect::GetTypeDesc(int index) const
	{
		ID3DX11EffectType* type = variables[index]->GetType();
		ASSERT(nullptr != type);

		D3DX11_EFFECT_TYPE_DESC typeDesc;
		D3D_VERIFY(type->GetDesc(&typeDesc));
		return typeDesc;
	}

#endif

	void Effect::SetVariableValue(int index, float value)
	{
#ifdef _DEBUG
		const D3DX11_EFFECT_TYPE_DESC typeDesc = GetTypeDesc(index);

		ASSERT_MSG(
			typeDesc.Type == D3D10_SVT_FLOAT && typeDesc.Class == D3D10_SVC_SCALAR && typeDesc.Elements == 0,
			"Effect variable type mismatch: expected scalar float.");
#endif

		ID3DX11EffectScalarVariable* scalar = variables[index]->AsScalar();
		ASSERT(scalar->IsValid());

		const HRESULT hRes = scalar->SetFloat(value);
		D3D_ASSERT(hRes);
	}

	void Effect::SetVariableValue(int index, const Matrix4& value)
	{
#ifdef _DEBUG
		const D3DX11_EFFECT_TYPE_DESC typeDesc = GetTypeDesc(index);

		ASSERT_MSG(
			typeDesc.Type == D3D10_SVT_FLOAT && typeDesc.Class == D3D10_SVC_MATRIX_COLUMNS && 
			typeDesc.Columns == 4 && typeDesc.Rows == 4 && typeDesc.Elements == 0, 
			"Effect variable type mismatch: expected matrix.");
#endif

		ID3DX11EffectMatrixVariable* matrix = variables[index]->AsMatrix();
		ASSERT(matrix->IsValid());

		const HRESULT hRes = matrix->SetMatrix((float*) &value);
		D3D_ASSERT(hRes);
	}

	void Effect::SetVariableValue(int index, const Vector2& value)
	{
#ifdef _DEBUG
		const D3DX11_EFFECT_TYPE_DESC typeDesc = GetTypeDesc(index);

		ASSERT_MSG(
			typeDesc.Type == D3D10_SVT_FLOAT && typeDesc.Class == D3D10_SVC_VECTOR && 
			typeDesc.Elements == 0 && typeDesc.Rows == 1 && typeDesc.Columns == 2, 
			"Effect variable type mismatch: expected Vector2.");
#endif

		ID3DX11EffectVectorVariable* vector = variables[index]->AsVector();
		ASSERT(vector->IsValid());

		const HRESULT hRes = vector->SetFloatVector((float*) &value);
		D3D_ASSERT(hRes);
	}

	void Effect::SetVariableValue(int index, const Vector4& value)
	{
#ifdef _DEBUG
		const D3DX11_EFFECT_TYPE_DESC typeDesc = GetTypeDesc(index);

		ASSERT_MSG(
			typeDesc.Type == D3D10_SVT_FLOAT && typeDesc.Class == D3D10_SVC_VECTOR && 
			typeDesc.Elements == 0 && typeDesc.Rows == 1 && typeDesc.Columns == 4, 
			"Effect variable type mismatch: expected Vector4.");
#endif

		ID3DX11EffectVectorVariable* vector = variables[index]->AsVector();
		ASSERT(vector->IsValid());

		const HRESULT hRes = vector->SetFloatVector(reinterpret_cast<const float*>(&value));
		D3D_ASSERT(hRes);
	}

	void Effect::SetVariableValue(int index, ID3D11ShaderResourceView* shaderResource)
	{
#ifdef _DEBUG
		const D3DX11_EFFECT_TYPE_DESC typeDesc = GetTypeDesc(index);

		ASSERT_MSG(
			(typeDesc.Type == D3D10_SVT_TEXTURE2D || typeDesc.Type == D3D10_SVT_TEXTURE3D) && typeDesc.Elements == 0, 
			"Effect variable type mismatch: expected Texture2D or Texture2D SRV.");
#endif

		ID3DX11EffectShaderResourceVariable* resource = variables[index]->AsShaderResource();
		ASSERT(resource->IsValid());

		const HRESULT hRes = resource->SetResource(shaderResource);
		D3D_ASSERT(hRes);
	}

	void Effect::SetVariableValue(int index, Vector4* value, int numValues)
	{
#ifdef _DEBUG
		const D3DX11_EFFECT_TYPE_DESC typeDesc = GetTypeDesc(index);

		ASSERT_MSG(
			typeDesc.Type == D3D10_SVT_FLOAT && typeDesc.Class == D3D10_SVC_VECTOR && 
			typeDesc.Elements == numValues && typeDesc.Rows == 1 && typeDesc.Columns == 4, 
			"Effect variable type mismatch: expected properly sized array of float4.");
#endif

		ID3DX11EffectVectorVariable* array = variables[index]->AsVector();
		ASSERT(array->IsValid());

		const HRESULT hRes = array->SetFloatVectorArray(reinterpret_cast<float*>(value), 0, numValues);
		D3D_ASSERT(hRes);
	}


	void Effect::SetVariableValue(int index, float* value, int numValues)
	{
#ifdef _DEBUG
		const D3DX11_EFFECT_TYPE_DESC typeDesc = GetTypeDesc(index);

		ASSERT_MSG( 
			typeDesc.Type == D3D10_SVT_FLOAT && typeDesc.Class == D3D10_SVC_SCALAR && typeDesc.Elements == numValues, 
			"Effect variable type mismatch: expected properly sized array of float4.");
#endif

		ID3DX11EffectScalarVariable* array = variables[index]->AsScalar();
		ASSERT(array->IsValid());

		const HRESULT hRes = array->SetFloatArray(value, 0, numValues);
		D3D_ASSERT(hRes);
	}
}
