#pragma once

#include "Node.h"

#include "Material.h"
#include "Camera.h"

namespace Pimp
{
	class Scene : public Element
	{
	private:
		Material* material; // can be NULL

		static float sceneRenderLOD; // 1 = full screen, 0.5 = half size, 0.25 = quarter of screen, etc...

	public:
		Scene(World* ownerWorld);
		virtual ~Scene() {}

		void Render(Camera* camera);

		void SetMaterial(Material* mat)
		{
			material = mat;
		}

		Material* GetMaterial() const 
		{
			return material;
		}

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