
#include "Platform.h"
#include "Node.h"

#define PIMP_MAX_NUM_NODE_CHILDREN 1024
#define PIMP_MAX_NUM_NODE_PARENTS  4096

namespace Pimp 
{
	Node::Node(World* ownerWorld)
		: Element(ownerWorld), parents(PIMP_MAX_NUM_NODE_PARENTS), children(PIMP_MAX_NUM_NODE_CHILDREN)
	{
		SetType(ET_Node);
		SetIsNode(true);
	}

	void Node::Tick(float deltaTime)
	{
		for (int i=children.Size()-1; i>=0; --i)
		{
			children[i]->Tick(deltaTime);
		}
	}

	const Matrix4* Node::GetWorldTransform() const
	{
		if (parents.Size() > 0)
		{
			ASSERT( parents.Size() == 1 );

			return parents[0]->GetWorldTransform();
		}
		else
			return Matrix4::GetIdentityPtr();
	}

	const Matrix4* Node::GetWorldTransformInv() const
	{
		if (parents.Size() > 0)
		{
			ASSERT( parents.Size() == 1 );

			return parents[0]->GetWorldTransformInv();
		}
		else
			return Matrix4::GetIdentityPtr();
	}

	void Node::CreateLookAt( Matrix4* outLookAt ) const
	{
		const Matrix4* wurld = GetWorldTransform();

		Vector3 worldPosition = Vector3(wurld->_41, wurld->_42, wurld->_43);
		Matrix4 o = wurld->InvertedOrthogonal();

		const Vector3 worldDirectionUp(o._12, o._22, o._32);
		const Vector3 worldDirectionForward(-o._13, -o._23, -o._33);

		*outLookAt = CreateMatrixLookAt(worldPosition, worldDirectionForward, worldDirectionUp);
	}
}