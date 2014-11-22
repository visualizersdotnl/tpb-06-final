
#pragma once

namespace Pimp 
{
	// Element types. This is a unique index per class, used for 
	// shameless reflection-like constructions.
	enum ElementType
	{
		ET_Element = 0,
		ET_Node,
		ET_Geometry,
		ET_Xform,
		ET_Camera,
		ET_AnimCurve,
		ET_PolyMesh,
		ET_MaterialParameter,
		ET_ParticleSpline,
		ET_ParticleAttractor,
		ET_Scene,
		ET_Metaballs,

		MAX_ElementType
	};
}
