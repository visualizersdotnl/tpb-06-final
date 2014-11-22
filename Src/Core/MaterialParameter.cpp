
#include "Platform.h"
#include "D3D.h"
#include "MaterialParameter.h"
#include "World.h"

namespace Pimp
{
	MaterialParameter::MaterialParameter(World* ownerWorld)
		: Node(ownerWorld), value(0.f), valueType(VT_Value)
	{
		SetType(ET_MaterialParameter);
		name[0] = 0;
	}

	void MaterialParameter::AssignValueToEffectVariable(Effect* effect, int varIndex)
	{
#ifdef _DEBUG
		const D3D10_EFFECT_TYPE_DESC desc = effect->GetTypeDesc(varIndex);

		// Safety checks on the type
		switch (valueType)
		{
		// Scalar float
		case VT_Value:
			if (desc.Class != D3D10_SVC_SCALAR || desc.Type != D3D10_SVT_FLOAT || desc.Elements != 0)
			{
				ASSERT(0);
				return;
			}
			break;

		// Vector
		case VT_NodePosition:
			if (desc.Class != D3D10_SVC_VECTOR || desc.Type != D3D10_SVT_FLOAT || desc.Elements != 0 || desc.Rows != 1 || desc.Columns != 4)
			{
				ASSERT(0);
				return;
			}
			break;

		// Matrix
		case VT_NodeXform:
		case VT_NodeXformInv: 
			if (desc.Class != D3D10_SVC_MATRIX_COLUMNS || desc.Type != D3D10_SVT_FLOAT || desc.Elements != 0 || desc.Rows != 4 || desc.Columns != 4)
			{
				ASSERT(0);
				return;
			}
			break;
		}
#endif

		switch (valueType)
		{
		case VT_Value:
			effect->SetVariableValue(varIndex, GetCurrentValue());
			break;
		case VT_NodePosition:
			effect->SetVariableValue(varIndex, *(Vector4*)&GetWorldTransform()->_41);
			break;
		case VT_NodeXform:
			effect->SetVariableValue(varIndex, *GetWorldTransform());
			break;
		case VT_NodeXformInv:
			effect->SetVariableValue(varIndex, *GetWorldTransformInv());
			break;
		default:
			ASSERT(0);
		}
	}
}
