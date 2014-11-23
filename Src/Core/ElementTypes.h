
#pragma once

namespace Pimp 
{
	// Element types. This is a unique index per class, used for 
	// shameless reflection-like constructions.
	enum ElementType
	{
		ET_Element = 0,
		ET_Node,
		ET_Xform,
		ET_Camera,
		ET_MaterialParameter,
		ET_Scene,

		MAX_ElementType
	};
}
