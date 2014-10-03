#include "MaterialParameter.h"
#include "world.h"


namespace Pimp
{
	MaterialParameter::MaterialParameter(World* ownerWorld)
		: Node(ownerWorld), animCurveValue(NULL), value(0), valueType(VT_Value)
	{
		SetType(ET_MaterialParameter);
		name[0] = 0;
	}

	void MaterialParameter::AssignValueToEffectVariable(Effect* effect, int varIndex)
	{
#ifdef _DEBUG
		D3D10_EFFECT_TYPE_DESC desc = effect->GetTypeDesc(varIndex);

		// Safety checks on the type
		switch (valueType)
		{
		case VT_Value: // float
			if (desc.Class != D3D10_SVC_SCALAR || desc.Type != D3D10_SVT_FLOAT || desc.Elements != 0)
				return;
			break;
		case VT_NodePosition: // float4
			if (desc.Class != D3D10_SVC_VECTOR || desc.Type != D3D10_SVT_FLOAT || desc.Elements != 0 || desc.Rows != 1 || desc.Columns != 4)
				return;
			break;
		case VT_NodeXform: // Matrix4
		case VT_NodeXformInv: // Matrix4
			if (desc.Class != D3D10_SVC_MATRIX_COLUMNS || desc.Type != D3D10_SVT_FLOAT || desc.Elements != 0 || desc.Rows != 4 || desc.Columns != 4)
				return;
		}
#endif


		switch (valueType)
		{
		case VT_Value: // float
			effect->SetVariableValue(varIndex, GetCurrentValue());
			break;
		case VT_NodePosition: // float4
			effect->SetVariableValue(varIndex, *(Vector4*)&GetWorldTransform()->_41);
			break;
		case VT_NodeXform: // Matrix4
			effect->SetVariableValue(varIndex, *GetWorldTransform());
			break;
		case VT_NodeXformInv: // Matrix4
			effect->SetVariableValue(varIndex, *GetWorldTransformInv());
			break;
		default:
			ASSERT( false );
		}
	}

}