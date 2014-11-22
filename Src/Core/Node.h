
#pragma once

#include <Shared/assert.h>
#include <Shared/fixedsizelist.h>
#include <Math/math.h>
#include "Element.h"

namespace Pimp 
{
	class Node 
		: public Element
	{
	protected:
		// FIXME: replace for STL.
		FixedSizeList<Node*> children;
		FixedSizeList<Node*> parents;

	public:
		Node(World* ownerWorld);
		virtual ~Node() {}

		FixedSizeList<Node*>& GetChildren()
		{
			return children;
		}

		FixedSizeList<Node*>& GetParents() 
		{
			return parents;
		}

		virtual const Matrix4* GetWorldTransform() const;
		virtual const Matrix4* GetWorldTransformInv() const;

		void CreateLookAt( Matrix4* outLookAt ) const;

		virtual void Tick(float deltaTime);
	};
}
