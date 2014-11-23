
#include <Core/Platform.h>
#include <Core/Core.h>
// #include "SceneTools.h"

// 
// Misc. scene graph helpers.
//

void AddChildToParent(Pimp::Node* nodeChild, Pimp::Node* nodeParent)
{
	Pimp::World::StaticAddChildToParent(nodeChild, nodeParent);
}

static void RecursiveRemoveNode(Pimp::Node* node)
{
	std::vector<Pimp::Element*>& elements = node->GetOwnerWorld()->GetElements();
	auto iElem = std::find(elements.begin(), elements.end(), node);
	if (elements.end() != iElem)
		elements.erase(iElem);

	for (auto iChild : node->GetChildren())
		RecursiveRemoveNode(iChild);
}

void RemoveNodeFromWorld(Pimp::Node* node)
{
	Pimp::World* world = node->GetOwnerWorld();

	for (auto iParent : node->GetParents())
	{
		std::vector<Pimp::Node*>& children = iParent->GetChildren();
		auto iNode = std::find(children.begin(), children.end(), node);
		if (children.end() != iNode)
			children.erase(iNode);

	}
	
	RecursiveRemoveNode(node);
}

void DuplicateTransformTransformedHierarchy(Pimp::World* world, Pimp::Node* from, Pimp::Node* to)
{
	ASSERT(nullptr != world && nullptr != from && nullptr != to);

	for (auto iChild : from->GetChildren())
	{
		if (Pimp::ET_Xform == iChild->GetType())
		{
			Pimp::Xform* copy = new Pimp::Xform(*static_cast<Pimp::Xform*>(iChild));
			world->GetElements().push_back(copy);
			AddChildToParent(copy, to);

			// Recurse..
			DuplicateTransformTransformedHierarchy(world, iChild, copy);
		}
	}
}

// Easy loading bar update & draw.
void DrawLoadProgress(Pimp::World &world, float progress)
{
	if (progress > 1.f)
		progress = 1.f;

	// Clear entire back buffer
	Pimp::gD3D->SetBackViewport();
	Pimp::gD3D->ClearBackBuffer();

	// Draw loading bar
	// It's still stuffed in the core post processing shader (FIXME: move it to the user one?).
	Pimp::gD3D->SetSceneViewport();
	Pimp::PostProcess* postProcess = world.GetPostProcess();
	postProcess->SetLoadProgress(progress);
	postProcess->Clear();
	postProcess->BindForRenderScene();
	postProcess->RenderPostProcess();

	// Flip it manually (blocking load)
	Pimp::gD3D->SetBackViewport();
	Pimp::gD3D->Flip(0);
}
