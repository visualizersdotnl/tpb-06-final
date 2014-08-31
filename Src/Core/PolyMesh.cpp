#include "PolyMesh.h"
#include <math/math.h>

#define PIMP_MAX_NUM_PACKEDINDICES 40000

namespace Pimp 
{
	PolyMesh::PolyMesh(World *ownerWorld)
		: Geometry(ownerWorld)
	{
		SetType(ET_PolyMesh);
	}

#ifdef _DEBUG
	PolyMesh::~PolyMesh()
	{
	}
#endif


	void PolyMesh::SetMeshData(
		int numVertices,
		int numIndices,
		Vector3* srcVertices,
		DWORD* srcIndices,
		int numSubdivisions,
		bool useHardNormals)
	{
		Vector3* newVertices;
		DWORD* newIndices;

		if (numSubdivisions > 0)
		{
			PerformLoopSubdivision(
				numVertices, numIndices,
				srcVertices, srcIndices,
				&newVertices, &newIndices,
				false);			

			for (int i=1; i<numSubdivisions; ++i)
			{
				PerformLoopSubdivision(
					numVertices, numIndices,
					newVertices, newIndices,
					&newVertices, &newIndices,
					true);
			}
		}
		else
		{
			newVertices = srcVertices;
			newIndices = srcIndices;
		}


		int numVerticesToGenerate = 
			(useHardNormals ? numIndices : numVertices);

//		this->indexed = !useHardNormals;

		InitBuffers(numVerticesToGenerate, numIndices);

		// Clear our buffers
		memset(vertices, 0, numVerticesToGenerate*sizeof(Vertex));

		// Copy indices
		memcpy(indices, newIndices, numIndices * sizeof(DWORD));

		// Copy vertex positions
		if (useHardNormals)
		{
			for (int i=0; i<numVerticesToGenerate; ++i)
				vertices[i].position = newVertices[indices[i]];
		}
		else
		{
			for (int i=0; i<numVerticesToGenerate; ++i)
				vertices[i].position = newVertices[i];

		}

		// Calculate face normals 
		for (int i=0; i<numIndices; i+=3)
		{
			int i0 = i, i1 = i+1, i2 = i+2;
			
			if (!useHardNormals)
			{
				i0 = indices[i0];
				i1 = indices[i1];
				i2 = indices[i2];
			}

			Vertex& a = vertices[i0];
			Vertex& b = vertices[i1];
			Vertex& c = vertices[i2];

			Vector3 ba, ca, n;

			ba = (b.position - a.position).Normalized();
			ca = (c.position - a.position).Normalized();

			n = ca.Cross(ba).Normalized();

			// add face normal to corner vertex normals
			a.normal += n;
			b.normal += n;
			c.normal += n;
		}

		// Calculate vertex normal space out of face normals
		for (int i=0; i<numVerticesToGenerate; ++i)
		{
			Vertex& v = vertices[i];

			// normalize normal
			v.normal.Normalize();

			//static const Vector3 worldUp(0,1,0);

			//// Calc tangent (side)
			//vn.tangent = v.normal.Cross(worldUp).Normalized();

			//// Calc binormal (up)
			//vn.binormal = vn.tangent.Cross(v.normal).Normalized();
		}


		if (numSubdivisions > 0)
		{
			delete [] newIndices;
			delete [] newVertices;
		}
	}


	struct EdgeEntry
	{
		int v1, v2;
		int adjecentFaceVertexIndices[2];
		int numAdjecentFaces;
		int edgeIndex;
		EdgeEntry* next;
	};

	void PolyMesh::PerformLoopSubdivision(
		int& numVertices, int& numIndices,
		Vector3* srcVertices, DWORD* srcIndices,
		Vector3** destVertices, DWORD** destIndices,
		bool shouldFreeDestFirst)
	{
		// We want to know which two (or less) faces belong to each set of 2 vertices that
		// are connected to each other.

		int numEdges = 0;
		EdgeEntry** edgesPerVertex = new EdgeEntry*[numVertices];
		memset(edgesPerVertex, 0, sizeof(EdgeEntry*)*numVertices);

		DWORD* edgeIndexPerIndex = new DWORD[numIndices];

		static const int offsetsNext[3] = {
			1, 1, -2
		};

		static const int offsetsPrev[3] = {
			2, -1, -1
		};

		for (int i=0; i<numIndices; i+=3)
		{
			for (int j=0; j<3; ++j)
			{
				// Edge from A to B, where C is the remaining vertex
				int a = srcIndices[i+j];
				int b = srcIndices[i+j+offsetsNext[j]];
				int c = srcIndices[i+j+offsetsPrev[j]];

				for (int k=0; k<2; ++k)
				{
					EdgeEntry* entry = edgesPerVertex[a];
					EdgeEntry** parentEntry = &edgesPerVertex[a];

					// Iterate through linked list until we either find it, or reach the end.
					while (entry != NULL && entry->v2 != b)
					{
						parentEntry = &entry->next;
						entry = entry->next;
					}				

					if (entry == NULL)
					{
						// We have no edge for these vertices at all. Create first entry.

						entry = new EdgeEntry;
						memset(entry, 0, sizeof(EdgeEntry));
						//entry->next = NULL;
						//entry->adjecentFaceVertexIndices[0]=0;
						//entry->adjecentFaceVertexIndices[1]=0;
						entry->numAdjecentFaces = 1;
						entry->adjecentFaceVertexIndices[0] = c;
						*parentEntry = entry;
						entry->edgeIndex = numEdges;

						numEdges += k; // Increment edge counter only for second store.

						entry->v1 = a;
						entry->v2 = b;
					}
					else
					{
						// We already had this edge.

#if 1
						if (entry->numAdjecentFaces > 1)
							entry->numAdjecentFaces = 1;
#endif

						entry->adjecentFaceVertexIndices[entry->numAdjecentFaces] = c;
						entry->numAdjecentFaces++;

#if 0
						ASSERT_MSG( entry->numAdjecentFaces <= 2, "Found an edge with more than two faces sharing it?!" );
#endif
					}

					// Swap a and b, so we safe the same edge again
					int tmp = a;
					a = b;
					b = tmp;

					//ASSERT_MSG( numVertices + entry->edgeIndex < 65536, "Invalid edge index! We're getting a too high vertex index!" );

					// Store edge index for edge starting at this index
					edgeIndexPerIndex[i+j] = (DWORD)(numVertices + entry->edgeIndex);
				}
			}
		}



		// Allocate new buffers
		int newNumVertices = numVertices + numEdges;
		int newNumIndices = numIndices * 4;

		DWORD* newIndices = new DWORD[newNumIndices];
		Vector3* newVertices = new Vector3[newNumVertices];

		// Vertexbuffer layout:
		// |-- original vertices --|-- edge points --|

		memcpy(newVertices, srcVertices, sizeof(Vector3)*numVertices);

		// Calculate edge and vertex points
		for (int i=0; i<numVertices; ++i)
		{
			EdgeEntry* entry = edgesPerVertex[i];

			int numNeighbourVertices = 0;
			Vector3 averageNeighbourPosition(0,0,0);

			while (entry != NULL)
			{
				// Only use entry with lowest v1, since each actual edge is present two
				// times in our list.
				if (i <= entry->v2)
				{
					static const float tableWeights[6] = { 
						0.5f, 0.0f, 
						0.5f, 0.0f, 
						3.0f / 8.0f, 1.0f / 8.0f };

					float weightEdgePoint = tableWeights[entry->numAdjecentFaces*2];
					float weightFaceVertex = tableWeights[entry->numAdjecentFaces*2 +1];

					// Add both end points
					Vector3 avgEdgePoint = (srcVertices[entry->v1] + srcVertices[entry->v2]) * weightEdgePoint;

					// Add the two vertices that together with our two edge points 
					// form the two triangles that share this edge.
					Vector3 avgFaceVertex = (srcVertices[entry->adjecentFaceVertexIndices[0]] + srcVertices[entry->adjecentFaceVertexIndices[1]]) * weightFaceVertex;

					// Sum both averages
					Vector3 edgePoint = avgEdgePoint + avgFaceVertex;

					// Store it
					newVertices[numVertices+entry->edgeIndex] = edgePoint;
				}	

				// Add neighbour vertex to average
				++numNeighbourVertices;
				averageNeighbourPosition += srcVertices[entry->v2];

				entry = entry->next;
			}

			// Calculate neighbourweights, using simplified weight function as 
			// specified by Warren and Weimer.
			float neighbourWeight = 3.0f / (float)(numNeighbourVertices*(numNeighbourVertices+2));
			float originalWeight = 1.0f - neighbourWeight * (float)numNeighbourVertices;

			// Calculate new position for our vertex
			Vector3 weightedOriginal = srcVertices[edgesPerVertex[i]->v1] * originalWeight;
			Vector3 weightedAverageNeighbours = averageNeighbourPosition * neighbourWeight;
			
			newVertices[edgesPerVertex[i]->v1] = weightedOriginal + weightedAverageNeighbours;
		}

		// Now create new indices too!

		DWORD* fingerIndices = srcIndices;
		DWORD* fingerEdgeIndices = edgeIndexPerIndex;
		DWORD* fingerNewIndices = newIndices;

		for (int i=0; i<numIndices; i+=3)
		{
			// A,B,C,
			// Edgepoint AB,
			// Edgepoint BC,
			// Edgepoint CA
			DWORD smoothedIndices[6];

			memcpy(smoothedIndices, fingerIndices, sizeof(DWORD)*3);
			memcpy(&smoothedIndices[3], fingerEdgeIndices, sizeof(DWORD)*3);

			static const int newIndexMappings[] = {
				0, 3, 5, // A - AB - CA
				1, 4, 3, // B - BC - AB
				2, 5, 4, // C - CA - BC
				3, 4, 5	 // AB - BC - CA
			};

			for (int j=0; j<12; ++j)
			{
				*(fingerNewIndices++) = smoothedIndices[newIndexMappings[j]];
			}

			fingerIndices += 3;
			fingerEdgeIndices += 3;
		}


		// Clean up

		for (int i=0; i<numVertices; ++i)
		{
			EdgeEntry* entry = edgesPerVertex[i];

			while (entry != NULL)
			{
				EdgeEntry* next = entry->next;
				delete entry;
				entry = next;
			}
		}

		delete [] edgesPerVertex;
		delete [] edgeIndexPerIndex;

		// Return the new numbers

		numVertices = newNumVertices;
		numIndices = newNumIndices;

		if (shouldFreeDestFirst)
		{
			delete [] *destVertices;
			delete [] *destIndices;
		}

		*destVertices = newVertices;
		*destIndices = newIndices;
	}


	void PolyMesh::SetPackedMeshData(
		const unsigned char* packedData,
		const Vector3& bboxMin,
		const Vector3& bboxMax,
		int numVertices,
		int indexStreamSize,
		int numSubdivisions,
		bool useHardNormals)
	{
		Vector3* vertices = new Vector3[numVertices];
		FixedSizeList<DWORD> indices(PIMP_MAX_NUM_PACKEDINDICES);

		const float* bboxLocalMinFinger = (float*) &bboxMin.x;
		const float* bboxLocalMaxFinger = (float*) &bboxMax.x;

		// Unpack vertices
		
		const char* signedPackedData = (const char*) packedData;

		for (int component=0; component<3; ++component)
		{
			float bboxRange = bboxLocalMaxFinger[component] - bboxLocalMinFinger[component];

			unsigned char previous = 0;
			for (int i=0; i<numVertices; ++i)
			{
				previous += *(signedPackedData++); // add delta

				float alpha = (float) previous / 127.0f;

				float* vertexFinger = (float*)(&(vertices[i].x));

				vertexFinger[component] = bboxLocalMinFinger[component] + alpha * bboxRange;
			}
		}

		// Unpack indices
		
		int indicesCache0, indicesCache1, indicesCache2;
		int index = 0;   
		int numBerOfIndices = 0;

//#ifdef _DEBUG
		indicesCache0 = indicesCache1 = indicesCache2 = 0;
//#endif

		for (int elementIndex=0; elementIndex<indexStreamSize; ++elementIndex)
		{
			unsigned char part1 = *signedPackedData;
			unsigned char part2 = *(signedPackedData+indexStreamSize);

			if (part2 != 0xff)
			{
				// Valid index!
				int indexDelta = (part1 & 0x7f) + (part2 << 7);
				if (part1 & 0x80)
					indexDelta = -indexDelta;

				index += indexDelta; 

				++numBerOfIndices;

				// store index
				indicesCache2 = indicesCache1;
				indicesCache1 = indicesCache0;
				indicesCache0 = index;

				if (numBerOfIndices >= 3 &&
					indicesCache0 != indicesCache1 &&
					indicesCache0 != indicesCache2 &&
					indicesCache1 != indicesCache2)
				{
					indices.Add(indicesCache0);   

					bool alternate = (numBerOfIndices%2 == 0);
					// store indices            
					indices.Add(alternate ? indicesCache1 : indicesCache2);
					indices.Add(alternate ? indicesCache2 : indicesCache1);					
				}
			}
			else
			{
				// End of segment -- start new one.

				ASSERT_MSG(part1 == 0, "Only triangle strips are supported atm for polygonal meshes!");

				index = 0;
				numBerOfIndices = 0;
			}


			++signedPackedData;
		}


		// Set our mesh!
		SetMeshData(numVertices, indices.Size(), vertices, indices.GetItemsPtr(), numSubdivisions, useHardNormals);
	}
}