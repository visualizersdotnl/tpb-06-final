#pragma once

#include "ElementTypes.h"

namespace Pimp 
{
	class World;

	class Element 
	{
	protected:
		World* ownerWorld;
		ElementType type;
		bool isNode;

		void SetType(ElementType type);
		void SetIsNode(bool isNode);

	public:
		Element(World* ownerWorld);
#ifdef _DEBUG
		virtual ~Element();
#endif

		virtual void Tick(float deltaTime) {}

		World* GetOwnerWorld() const 
		{
			return ownerWorld;
		}

		virtual bool HasGeometry() const 
		{
			return false;
		}

		ElementType GetType() const 
		{
			return type;
		}

		bool IsNode() const 
		{
			return isNode;
		}

		virtual void ForceSetTime(float time) {}
	};
}