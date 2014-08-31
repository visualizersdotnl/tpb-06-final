#include "Geometry.h"
#include "D3D.h"

namespace Pimp 
{
	Geometry::Geometry(World *ownerWorld)
		: Node(ownerWorld), 
		vertices(0), indices(0),
		numVertices(0), numIndices(0),
		isVisible(true)
	{
		SetType(ET_Geometry);
	}

#ifdef _DEBUG
	Geometry::~Geometry()
	{
		delete [] vertices;
		delete [] indices;
	}
#endif

	void Geometry::InitBuffers(int numVertices, int numIndices)
	{
		if (vertices)
			delete [] vertices;

		if (indices)
			delete [] indices;

		this->numVertices = numVertices;
		this->numIndices = numIndices;

		vertices = new Vertex[numVertices];
		indices = new DWORD[numIndices];
	}
}
