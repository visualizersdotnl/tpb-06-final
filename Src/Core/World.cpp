
#include "Platform.h"
#include "D3D.h"
#include "World.h"
#include "Xform.h"

// Non-node drawables.
#include "Metaballs.h"
#include "Sprites.h"

// FIXME: Introduce regular STL vectors instead?
#define PIMP_MAX_NUM_WORLD_ELEMENTS 8192
#define PIMP_MAX_NUM_TEXTURES 256
#define PIMP_MAX_NUM_SCENES 256
#define PIMP_MAX_NUM_MATERIALS 256

namespace Pimp 
{
	World::World()
		: elements(PIMP_MAX_NUM_WORLD_ELEMENTS)
,		currentCamera(nullptr)
,		currentSceneIndex(-1)
,		textures(PIMP_MAX_NUM_TEXTURES)
,		materials(PIMP_MAX_NUM_MATERIALS)
,		scenes(PIMP_MAX_NUM_SCENES)
,		postProcess(new PostProcess())
,		screenQuadVertexBuffer(new ScreenQuadVertexBuffer(postProcess->GetEffectPassBloomGather()))
,		currentTime(0.f)
,		motionBlurAmount(0.f)
,		prevMotionBlurTime(9999999.0)
	{
		rootNode = new Xform(this);
		elements.Add(rootNode);

		// Bind for loading bar (embedded in core post processing shader).
		screenQuadVertexBuffer->Bind();
	}

	World::~World()
	{
		for (int i=0; i<elements.Size(); ++i)
			delete elements[i];

		delete screenQuadVertexBuffer;
		delete postProcess;
	}

	void World::Tick(float deltaTime)
	{
		currentTime += deltaTime;

		// Tick ALL non-node elements first
		for (int i=elements.Size()-1; i>=0; --i)
		{
			if (!elements[i]->IsNode())
			{
				elements[i]->Tick(deltaTime);
			}			
		}

		// Then tick our DAG with nodes
		rootNode->Tick(deltaTime);

		// Handle motion blur
		const float timeSincePrevMotionBlur = currentTime - prevMotionBlurTime;
		if (timeSincePrevMotionBlur <= 0.f)
		{
			// Reset motion blurring since we're not moving forward on the timeline *or* we just switched cameras
			postProcess->SetMotionBlurFrameWeight(1.f);
		}
		else
		{
			// Calculate weight of current frame. 
			// Previous frame will have a weight of 1 - currentframeweight. 
			// Note that we have to use pow() since it's an exponential function. 
			// The motionBlurAmount that is specified is an amount when our framerate is exactly 60Hz.
			const float currentFrameWeight = (motionBlurAmount > 0) ? (1.f - powf(motionBlurAmount, timeSincePrevMotionBlur*60.f)) : 1.f;
			postProcess->SetMotionBlurFrameWeight(currentFrameWeight);
		}

		prevMotionBlurTime = currentTime;
	}

	void World::Render(Sprites &sprites, Metaballs *pMetaballs)
	{
		// FIXME: maybe clear entire back buffer here?

		// Set scene viewport
		gD3D->SetSceneViewport();

		// Clear scene target
		postProcess->Clear(); 

		// Bind scene target
		postProcess->BindForRenderScene();
		{
			// Disable depth stencil
			gD3D->DisableDepthStencil();

			// Prepare all sprites to be drawn
			sprites.PrepareToDraw();

			// Draw background sprites
			sprites.DrawBackgroundSprites();

			// Bind screen quad VB for first pass
			screenQuadVertexBuffer->Bind();

			// Bind camera
			if (currentCamera != NULL)
				currentCamera->Bind();

			// Render our scene to a single sceneColor (? -> FIXMEM, ask Glow) FP16 RT
			if (scenes.IsValidIndex(currentSceneIndex) && 
				scenes[currentSceneIndex] != NULL)
			{
				scenes[currentSceneIndex]->Render(currentCamera);
			}

			// Draw metaballs? (FIXME?)
			if (nullptr != pMetaballs)
			{
				// Clear & enable depth stencil
				gD3D->ClearDepthStencil();
				gD3D->EnableDepthStencil();

				pMetaballs->Draw(currentCamera);

				// Disable depth stencil
				gD3D->DisableDepthStencil();
			}
		}

		// Bind screen quad VB for RenderPostProcess()
		screenQuadVertexBuffer->Bind();

		// Combine all posteffects to back buffer
		postProcess->RenderPostProcess();

		// ** At this point, the back buffer will be bound  **

		// Set adjusted back buffer viewport for spritess
//		gD3D->SetBackAdjViewport();
		// ^^ Already done by PostProcess::RenderPostProcess()

		// Flush (draw & clear queue) the sprites
		sprites.DrawSprites();

		// Set full back buffer viewport
		gD3D->SetBackViewport();

		// Ensure blend mode is none for next frame
		gD3D->SetBlendMode(D3D::BM_None);

		// When the demo is running, Player takes care of presenting the backbuffer,
		// so no need to call gD3D->Flip().
	}

	void World::ForceSetTime(float time)
	{
		currentTime = time;

		for (int i=0; i<elements.Size(); ++i)
		{
			elements[i]->ForceSetTime(time);
		}
	}

	void World::StaticAddChildToParent(Node* nodeChild, Node* nodeParent)
	{
		ASSERT(nodeChild != NULL);
		ASSERT(nodeParent != NULL);
		//	ASSERT(nodeChild->GetParent() == NULL);

		// Add child to parent's children
		nodeParent->GetChildren().Add(nodeChild);

		// Add parent to child's parents
		nodeChild->GetParents().Add(nodeParent);
	}

	void World::StaticRemoveChildFromParent(Node* nodeChild, Node* nodeParent)
	{
		ASSERT(nodeChild != NULL);
		ASSERT(nodeParent != NULL);
		//	ASSERT(nodeChild->GetParent() == NULL);

		// Remove child from parent's children
		nodeParent->GetChildren().Remove(nodeChild);

		// Remove parent from child's parents
		nodeChild->GetParents().Remove(nodeParent);
	}

	void World::UpdateAllMaterialParameters()
	{
		for (int i=0; i<materials.Size(); ++i)
			if (materials[i] != NULL)
				materials[i]->RefreshParameters();
	}

	void World::SetMotionBlurAmount( float amount )
	{
		motionBlurAmount = amount;
	}
}
