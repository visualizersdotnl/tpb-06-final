
#pragma once

//#include <Shared/shared.h>
#include "Node.h"
#include "Effect.h"

namespace Pimp 
{
	class MaterialParameter : public Node
	{
	public:
		enum ValueType
		{
			VT_Value = 0,
			VT_NodePosition,
			VT_NodeXform,
			VT_NodeXformInv
		};
	private:
		float value;
		char name[512];
		ValueType valueType;

	public:
		MaterialParameter(World* ownerWorld);		
		virtual ~MaterialParameter() {}

		float GetCurrentValue() const 
		{
			return value;
		}

		void SetValue(float value)
		{
			this->value = value;
		}

		void SetName(const char* name)
		{
			strcpy(this->name, name);
		}

		const char* GetName() const 
		{
			return name;
		}

		void SetValueType(int type)
		{
			valueType = (ValueType)type;
		}

		void AssignValueToEffectVariable(Effect* effect, int varIndex);
	};
}