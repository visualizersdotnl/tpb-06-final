
#pragma once

#include "Node.h"
#include "Material.h"
#include "Camera.h"

namespace Pimp
{
	class Scene : public Element
	{
	private:
		// Check impl.
		static float sceneRenderLOD;

		Material* material;

	public:
		Scene(World* ownerWorld);
		virtual ~Scene() {}

		void Render(Camera* camera);

		void SetMaterial(Material* material) { this->material = material;  }
		Material* GetMaterial() const        { return material;            }

		static float GetSceneRenderLOD() 
		{
			return sceneRenderLOD;
		}

		static void SetSceneRenderLOD(float value)
		{
			sceneRenderLOD = value;
		}
	};
}
