
// FIXME:
// Scene currently keeps a pointer whilst given a reference, so that Player's AutoShaderReload mechanism
// can swap the material on reload. This obviously isn't pretty and I should look after a proper solution.

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

		// FIXME!
		Material* GetMaterial() const         { return material; }
		void SetMaterial(Material* material)  { this->material = material; }

#endif

	};
}
