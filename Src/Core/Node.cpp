
#include "Platform.h"
#include "Node.h"

namespace Pimp 
{
	Node::Node(World* ownerWorld) : Element(ownerWorld)
	{
		SetType(ET_Node);
		SetIsNode(true);
	}

	void Node::Tick(float deltaTime)
	{
		for (auto iChild = children.rbegin(); iChild != children.rend(); ++iChild)
			(*iChild)->Tick(deltaTime);
	}

	const Matrix4* Node::GetWorldTransform() const
	{
		if (false == parents.empty())
		{
			ASSERT(1 == parents.size());
			return parents[0]->GetWorldTransform();
		}
		else
			return Matrix4::GetIdentityPtr();
	}

	const Matrix4* Node::GetWorldTransformInv() const
	{
		if (false == parents.empty())
		{
			ASSERT(1 == parents.size());
			return parents[0]->GetWorldTransformInv();
		}
		else
			return Matrix4::GetIdentityPtr();
	}

	void Node::CreateLookAt( Matrix4* outLookAt ) const
	{
		const Matrix4* mWorld = GetWorldTransform();

		const Vector3 worldPosition = Vector3(mWorld->_41, mWorld->_42, mWorld->_43);
		const Matrix4 mOrientation = mWorld->InvertedOrthogonal();

		const Vector3 worldDirectionUp(mOrientation._12, mOrientation._22, mOrientation._32);
		const Vector3 worldDirectionForward(-mOrientation._13, -mOrientation._23, -mOrientation._33);

		*outLookAt = CreateMatrixLookAt(worldPosition, worldDirectionForward, worldDirectionUp);
	}
}