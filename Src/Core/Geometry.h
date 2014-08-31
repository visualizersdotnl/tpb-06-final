#pragma once

#include "Node.h"


namespace Pimp 
{
	class Geometry : 
		public Node 
	{
	protected:
		struct Vertex
		{
			Vector3 position;
			Vector3 normal;
		};

		bool isVisible;

		Vertex* vertices;
		DWORD* indices;
		int numVertices;
		int numIndices;

		void InitBuffers(int numVertices, int numIndices);

	public:
		Geometry(World* ownerWorld);

#ifdef _DEBUG
		virtual ~Geometry();
#endif		

		virtual bool HasGeometry() const 
		{
			return true;
		}

		bool ShouldRender() const 
		{
			return isVisible;
		}

		void SetVisible(bool visible) 
		{
			isVisible = visible;
		}
	};
}