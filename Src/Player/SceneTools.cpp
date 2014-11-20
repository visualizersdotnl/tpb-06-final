
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

static void AddAnimCurveKey(FixedSizeList<Pimp::AnimCurve::Pair>& keys, float time, float value)
{
	Pimp::AnimCurve::Pair key;

	key.time = time;
	key.value = value;
	key.inTangentY = key.outTangentY = 0;

	keys.Add(key);
}

static void RecursiveRemoveNode(Pimp::Node* node)
{
	FixedSizeList<Pimp::Element*>& elems = node->GetOwnerWorld()->GetElements();
	elems.Remove(node);

	for (int i=0; i<node->GetChildren().Size(); ++i)
		RecursiveRemoveNode(node->GetChildren()[i]);
}

void RemoveNodeFromWorld(Pimp::Node* node)
{
	Pimp::World* world = node->GetOwnerWorld();

	FixedSizeList<Pimp::Node*>& parents = node->GetParents();
	for (int i=0; i<parents.Size(); ++i)
	{
		parents[i]->GetChildren().Remove(node);
	}
	
	RecursiveRemoveNode(node);
}

void DuplicateTransformTransformedHierarchy(
	Pimp::World* w,
	Pimp::Node* source, Pimp::Node* dest)
{
	for (int i=0; i<source->GetChildren().Size(); ++i)
	{
		Pimp::Node* n = source->GetChildren()[i];

		if (n->GetType() == Pimp::ET_Xform)
		{
			Pimp::Xform* x = new Pimp::Xform(*(Pimp::Xform*)n);
			w->GetElements().Add(x);

			AddChildToParent(x, dest);

			DuplicateTransformTransformedHierarchy(w, n, x);
		}
		else if (n->GetType() == Pimp::ET_PolyMesh)
		{
			// Instance polymesh!
			AddChildToParent(n, dest);
		}
	}
}

// FIXME: do we still need this one?
Pimp::AnimCurve* DuplicateAnimCurve( Pimp::World* world, Pimp::AnimCurve* curve, float delay )
{
	Pimp::AnimCurve* dupeCurve = new Pimp::AnimCurve(world);
	world->GetElements().Add(dupeCurve);

	FixedSizeList<Pimp::AnimCurve::Pair>* pairs = new FixedSizeList<Pimp::AnimCurve::Pair>(curve->GetKeysPtr()->Size());

	*pairs = *curve->GetKeysPtr();

	for (int i=0; i<pairs->Size(); ++i)
	{
		Pimp::AnimCurve::Pair& pair = (*pairs)[i];
		pair.time += delay;
	}

	dupeCurve->SetKeysPtr(pairs);

	return dupeCurve;
}

// Easy loading bar update & draw.
void DrawLoadProgress(Pimp::World &world, float progress)
{
	if (progress > 1.f)
		progress = 1.f;

	Pimp::PostProcess* postProcess = world.GetPostProcess();
	postProcess->SetLoadProgress(progress);
	postProcess->Clear();
	postProcess->BindForRenderScene();
	postProcess->RenderPostProcess();
	Pimp::gD3D->Flip();
}
