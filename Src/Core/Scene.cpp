#include "Scene.h"

namespace Pimp
{
	// Adjust sceneRenderLOD to render all scenes at a lower res.
	// 1.0 = At exact screen resolution
	// 0.5 = Half res
	// 0.25 = Quarter res
	// etc...
	// Note that performance of our scene shaders typically scales linearly with 
	// the amount of rendered pixels.

#ifndef _DEBUG
	float Scene::sceneRenderLOD = 1.000f;
#else
	float Scene::sceneRenderLOD = 1.000f;
#endif

	Scene::Scene(World* ownerWorld) :
		Element(ownerWorld), material(NULL)
	{
		SetType(ET_Scene);
	}

	void Scene::Render(Camera* camera)
	{
		if (material != NULL)
		{
			material->Bind(camera);
			gD3D->DrawScreenQuad();
		}
	}
}