
#include "Platform.h"
#include "D3D.h"
#include "Scene.h"

namespace Pimp
{
	// Adjust sceneRenderLOD to render all scenes at a lower resolution.
	// - 1.0  = At exact screen resolution.
	// - 0.5  = Half.
	// - 0.25 = Quarter.
	//
	// Note that performance of our scene shaders typically scales linearly with the amount of rendered pixels.

#ifndef _DEBUG
	float Scene::sceneRenderLOD = 1.f;
#else
	float Scene::sceneRenderLOD = 1.f;
#endif

	Scene::Scene(World* ownerWorld, Material& material) :
		Element(ownerWorld)
,		material(&material)
,		screenQuad(material.GetEffectPass())
	{
		SetType(ET_Scene);
	}

	void Scene::Render(Camera* camera)
	{
		material->Bind(camera);
		gD3D->SetBlendMode(material->GetBlendMode());
		screenQuad.Bind();
		screenQuad.Draw();
		gD3D->SetBlendMode(D3D::Blend::BM_None);
	}
}
