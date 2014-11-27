
// Scene describes a full screen scene rendered by a material shader (FX).
// Needs expansion when actual geometry will be implemented.

#pragma once

#include "Node.h"
#include "Material.h"
#include "Camera.h"
#include "ScreenQuad.h"

namespace Pimp
{
	class Scene : public Element
	{
	private:
		// Check impl.
		static float sceneRenderLOD;

		Material* material;
		ScreenQuad screenQuad;

	public:
		Scene(World* ownerWorld, Material& material);
		~Scene() {}

		void Render(Camera* camera);

		static float GetSceneRenderLOD()           { return sceneRenderLOD;  }
		static void SetSceneRenderLOD(float value) { sceneRenderLOD = value; }

#if defined(_DEBUG) || defined(_DESIGN)

		// FIXME: AutoShaderReload (Player) can swap the material and needs a review because
		//        it's rather ugly.
		Material* GetMaterial() const         { return material; }
		void SetMaterial(Material* material)  { this->material = material; }

#endif

	};
}
