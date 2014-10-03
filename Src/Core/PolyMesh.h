#pragma once

#include "Node.h"

#include "Geometry.h"

namespace Pimp 
{
	class PolyMesh 
		: public Geometry 
	{
	private:

		void PerformLoopSubdivision(
			int& numVertices, int& numIndices,
			Vector3* srcVertices, DWORD* srcIndices,
			Vector3** destVertices, DWORD** destIndices,
			bool shouldFreeDestFirst);

	public:
		PolyMesh(World* ownerWorld);
		virtual ~PolyMesh() {}

		void SetMeshData(
			int numVertices,
			int numIndices,
			Vector3* vertices,
			DWORD* indices,
			int numSubdivisions,
			bool useHardNormals);

		void SetPackedMeshData(
			const unsigned char* packedData,
			const Vector3& bboxMin,
			const Vector3& bboxMax,
			int numVertices,
			int indexStreamSize,
			int numSubdivisions,
			bool useHardNormals);

	};
}