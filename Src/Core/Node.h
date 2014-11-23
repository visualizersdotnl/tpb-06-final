
#pragma once

#include <Math/math.h>
#include "Element.h"

namespace Pimp 
{
	class Node 
		: public Element
	{
	protected:
		std::vector<Node*> children;
		std::vector<Node*> parents;

	public:
		Node(World* ownerWorld);
		virtual ~Node() {}

		std::vector<Node*>& GetChildren() { return children; }
		std::vector<Node*>& GetParents()  { return parents;  }

		virtual const Matrix4* GetWorldTransform() const;
		virtual const Matrix4* GetWorldTransformInv() const;

		void CreateLookAt( Matrix4* outLookAt ) const;

		virtual void Tick(float deltaTime);
	};
}
