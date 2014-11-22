
#include "Platform.h"
#include "Element.h"

namespace Pimp 
{
	Element::Element(World* ownerWorld)
		: ownerWorld(ownerWorld), isNode(false), type(ET_Element)
	{
	}

	Element::~Element() {}

	void Element::SetType(ElementType type)
	{
		this->type = type;
	}

	void Element::SetIsNode(bool isNode)
	{
		this->isNode = isNode;
	}
}