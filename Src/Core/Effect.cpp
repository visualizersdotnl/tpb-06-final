
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

	ID3D10EffectTechnique* Effect::GetTechnique(const char* name)
	{
		ID3D10EffectTechnique* technique = effect->GetTechniqueByName(name);
		ASSERT(nullptr != technique);
		return technique->IsValid() ? technique : nullptr;
	}

	int Effect::RegisterVariable(const char* name, bool required)
	{
		ID3D10EffectVariable* variable = effect->GetVariableByName(name);
		ASSERT(!required || (nullptr != variable && variable->IsValid()));

		if (nullptr != variable && variable->IsValid())
		{
			variables.push_back(variable);
			return int(variables.size()-1);
		}
		else
		{
			return -1;
		}
	}

	bool Effect::HasVariable(const char* name) const
	{
		ID3D10EffectVariable* variable = effect->GetVariableByName(name);
		return nullptr != variable && variable->IsValid();
	}

	void Effect::ResetRegisteredVariables()
	{
		variables.clear();
	}

#ifdef _DEBUG

	const D3D10_EFFECT_TYPE_DESC Effect::GetTypeDesc(int index) const
	{
		ID3D10EffectType* type = variables[index]->GetType();
		ASSERT(type != NULL);

		D3D10_EFFECT_TYPE_DESC desc;
		D3D_VERIFY(type->GetDesc(&desc));

		return desc;
	}

#endif

	void Effect::SetVariableValue(int index, float value)
	{
#ifdef _DEBUG
		const D3D10_EFFECT_TYPE_DESC desc = GetTypeDesc(index);

		ASSERT_MSG( desc.Type == D3D10_SVT_FLOAT && desc.Class == D3D10_SVC_SCALAR && desc.Elements == 0,
			"Effect variable type mismatch: expected scalar float." )
#endif

		ID3D10EffectScalarVariable* scalar = variables[index]->AsScalar();
		ASSERT(scalar->IsValid());

		const HRESULT hRes = scalar->SetFloat(value);
		D3D_ASSERT(hRes);
	}

	void Effect::SetVariableValue(int index, const Matrix4& value)
	{
#ifdef _DEBUG
		const D3D10_EFFECT_TYPE_DESC desc = GetTypeDesc(index);

		ASSERT_MSG( desc.Type == D3D10_SVT_FLOAT && desc.Class == D3D10_SVC_MATRIX_COLUMNS && desc.Columns == 4 && desc.Rows == 4 && desc.Elements == 0, 
			"Effect variable type mismatch: expected matrix." )
#endif

		ID3D10EffectMatrixVariable* matrix = variables[index]->AsMatrix();
		ASSERT(matrix->IsValid());

		const HRESULT hRes = matrix->SetMatrix((float*) &value);
		D3D_ASSERT(hRes);
	}

	void Effect::SetVariableValue(int index, const Vector2& value)
	{
#ifdef _DEBUG
		const D3D10_EFFECT_TYPE_DESC desc = GetTypeDesc(index);

		ASSERT_MSG( desc.Type == D3D10_SVT_FLOAT && desc.Class == D3D10_SVC_VECTOR && desc.Elements == 0 && desc.Rows == 1 && desc.Columns == 2, 
			"Effect variable type mismatch: expected Vector2." )
#endif

		ID3D10EffectVectorVariable* vector = variables[index]->AsVector();
		ASSERT(vector->IsValid());

		const HRESULT hRes = vector->SetFloatVector((float*) &value);
		D3D_ASSERT(hRes);
	}

	void Effect::SetVariableValue(int index, const Vector4& value)
	{
#ifdef _DEBUG
		const D3D10_EFFECT_TYPE_DESC desc = GetTypeDesc(index);

		ASSERT_MSG( desc.Type == D3D10_SVT_FLOAT && desc.Class == D3D10_SVC_VECTOR && desc.Elements == 0 && desc.Rows == 1 && desc.Columns == 4, 
			"Effect variable type mismatch: expected Vector4." )
#endif

		ID3D10EffectVectorVariable* vector = variables[index]->AsVector();
		ASSERT(vector->IsValid());

		const HRESULT hRes = vector->SetFloatVector((float*) &value);
		D3D_ASSERT(hRes);
	}

	void Effect::SetVariableValue(int index, ID3D10ShaderResourceView* shaderResource)
	{
#ifdef _DEBUG
		const D3D10_EFFECT_TYPE_DESC desc = GetTypeDesc(index);

		ASSERT_MSG( (desc.Type == D3D10_SVT_TEXTURE2D || desc.Type == D3D10_SVT_TEXTURE3D) && desc.Elements == 0, 
			"Effect variable type mismatch: expected Texture2D or Texture2D SRV." )
#endif

		ID3D10EffectShaderResourceVariable* resource = variables[index]->AsShaderResource();
		ASSERT(resource->IsValid());

		const HRESULT hRes = resource->SetResource(shaderResource);
		D3D_ASSERT(hRes);
	}

	void Effect::SetVariableValue(int index, Vector4* value, int numValues)
	{
#ifdef _DEBUG
		const D3D10_EFFECT_TYPE_DESC desc = GetTypeDesc(index);

		ASSERT_MSG( desc.Type == D3D10_SVT_FLOAT && desc.Class == D3D10_SVC_VECTOR && desc.Elements == numValues && desc.Rows == 1 && desc.Columns == 4, 
			"Effect variable type mismatch: expected properly sized array of float4." )
#endif

		ID3D10EffectVectorVariable* array = variables[index]->AsVector();
		ASSERT(array->IsValid());

		const HRESULT hRes = array->SetFloatVectorArray((float*) value, 0, numValues);
		D3D_ASSERT(hRes);
	}


	void Effect::SetVariableValue(int index, float* value, int numValues)
	{
#ifdef _DEBUG
		const D3D10_EFFECT_TYPE_DESC desc = GetTypeDesc(index);

		ASSERT_MSG( desc.Type == D3D10_SVT_FLOAT && desc.Class == D3D10_SVC_SCALAR && desc.Elements == numValues, 
			"Effect variable type mismatch: expected properly sized array of float4." )
#endif

		ID3D10EffectScalarVariable* array = variables[index]->AsScalar();
		ASSERT(array->IsValid());

		const HRESULT hRes = array->SetFloatArray(value, 0, numValues);
		D3D_ASSERT(hRes);
	}
}
