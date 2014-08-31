#include "Effect.h"
#include "D3D.h"
#include <Shared/shared.h>

#define EFFECT_MAX_NUM_VARIABLES 256

namespace Pimp
{
	Effect::Effect(const unsigned char* compiledEffect, int compiledEffectLength)
		: variables(EFFECT_MAX_NUM_VARIABLES)
	{
		effect = gD3D->CreateEffect(compiledEffect, compiledEffectLength);
	}


	Effect::~Effect()
	{
		effect->Release();
	}

	ID3D10EffectTechnique* Effect::GetTechnique(const char* name)
	{
		ID3D10EffectTechnique* tech = effect->GetTechniqueByName(name);

		return tech->IsValid() ? tech : NULL;
	}

	int Effect::RegisterVariable(const char* name, bool required)
	{
		ID3D10EffectVariable* variable = effect->GetVariableByName(name);

		ASSERT(!required || (variable != NULL && variable->IsValid()));

		if (variable != NULL && variable->IsValid())
		{
			variables.Add(variable);

			return variables.Size()-1;
		}
		else
			return -1;
	}

	bool Effect::HasVariable(const char* name) const
	{
		ID3D10EffectVariable* variable = effect->GetVariableByName(name);

		return variable != NULL && variable->IsValid();
	}

	void Effect::ResetRegisteredVariables()
	{
		variables.Clear();
	}

#ifdef _DEBUG

	D3D10_EFFECT_TYPE_DESC Effect::GetTypeDesc(int index) const
	{
		ID3D10EffectType* type = variables[index]->GetType();
		ASSERT(type != NULL);

		D3D10_EFFECT_TYPE_DESC desc;
		D3D_VERIFY( type->GetDesc(&desc), "EffectType::GetDesc" );

		return desc;
	}

#endif

	void Effect::SetVariableValue( int index, float value )
	{
#ifdef _DEBUG
		D3D10_EFFECT_TYPE_DESC desc = GetTypeDesc(index);

		ASSERT_MSG( desc.Type == D3D10_SVT_FLOAT && desc.Class == D3D10_SVC_SCALAR && desc.Elements == 0,
			"Cannot set effect variable. It's not a single scalar float var." )
#endif

		ID3D10EffectScalarVariable* scalar = variables[index]->AsScalar();
		ASSERT(scalar->IsValid());

		HRESULT hr = scalar->SetFloat(value);
		D3D_ASSERT(hr);
	}

	void Effect::SetVariableValue( int index, const Matrix4& value )
	{
#ifdef _DEBUG
		D3D10_EFFECT_TYPE_DESC desc = GetTypeDesc(index);

		ASSERT_MSG( desc.Type == D3D10_SVT_FLOAT && desc.Class == D3D10_SVC_MATRIX_COLUMNS && desc.Columns == 4 && desc.Rows == 4 && desc.Elements == 0, 
			"Cannot set effect variable. It's not a matrix var." )
#endif

		ID3D10EffectMatrixVariable* matrix = variables[index]->AsMatrix();
		ASSERT(matrix->IsValid());

		HRESULT hr = matrix->SetMatrix((float*)&value);
		D3D_ASSERT(hr);
	}

	void Effect::SetVariableValue( int index, const Vector2& value )
	{
#ifdef _DEBUG
		D3D10_EFFECT_TYPE_DESC desc = GetTypeDesc(index);

		ASSERT_MSG( desc.Type == D3D10_SVT_FLOAT && desc.Class == D3D10_SVC_VECTOR && desc.Elements == 0 && desc.Rows == 1 && desc.Columns == 2, 
			"Cannot set effect variable. It's not a float2 var." )
#endif

		ID3D10EffectVectorVariable* var = variables[index]->AsVector();
		ASSERT(var->IsValid());

		HRESULT hr = var->SetFloatVector((float*)&value);
		D3D_ASSERT(hr);
	}

	void Effect::SetVariableValue( int index, const Vector4& value )
	{
#ifdef _DEBUG
		D3D10_EFFECT_TYPE_DESC desc = GetTypeDesc(index);

		ASSERT_MSG( desc.Type == D3D10_SVT_FLOAT && desc.Class == D3D10_SVC_VECTOR && desc.Elements == 0 && desc.Rows == 1 && desc.Columns == 4, 
			"Cannot set effect variable. It's not a float4 var." )
#endif

		ID3D10EffectVectorVariable* var = variables[index]->AsVector();
		ASSERT(var->IsValid());

		HRESULT hr = var->SetFloatVector((float*)&value);
		D3D_ASSERT(hr);
	}

	void Effect::SetVariableValue(int index, ID3D10ShaderResourceView* shaderResource)
	{
#ifdef _DEBUG
		D3D10_EFFECT_TYPE_DESC desc = GetTypeDesc(index);

		ASSERT_MSG( (desc.Type == D3D10_SVT_TEXTURE2D || desc.Type == D3D10_SVT_TEXTURE3D) && desc.Elements == 0, 
			"Cannot set effect variable. It's not a texture2D or texture3D var." )
#endif

		ID3D10EffectShaderResourceVariable* var = variables[index]->AsShaderResource();
		ASSERT(var->IsValid());

		HRESULT hr = var->SetResource(shaderResource);
		D3D_ASSERT(hr);
	}

	void Effect::SetVariableValue(int index, Vector4* value, int numValues)
	{
#ifdef _DEBUG
		D3D10_EFFECT_TYPE_DESC desc = GetTypeDesc(index);
		ASSERT_MSG( desc.Type == D3D10_SVT_FLOAT && desc.Class == D3D10_SVC_VECTOR && desc.Elements == numValues && desc.Rows == 1 && desc.Columns == 4, 
			"Cannot set effect variable. It's not a float4 array of the right size." )
#endif

		ID3D10EffectVectorVariable* var = variables[index]->AsVector();
		ASSERT(var->IsValid());

		HRESULT hr = var->SetFloatVectorArray((float*)value, 0, numValues);
		D3D_ASSERT(hr);
	}


	void Effect::SetVariableValue(int index, float* value, int numValues)
	{
#ifdef _DEBUG
		D3D10_EFFECT_TYPE_DESC desc = GetTypeDesc(index);
		ASSERT_MSG( desc.Type == D3D10_SVT_FLOAT && desc.Class == D3D10_SVC_SCALAR && desc.Elements == numValues, 
			"Cannot set effect variable. It's not a float array of the right size." )
#endif

		ID3D10EffectScalarVariable* var = variables[index]->AsScalar();
		ASSERT(var->IsValid());

		HRESULT hr = var->SetFloatArray(value, 0, numValues);
		D3D_ASSERT(hr);
	}

}

