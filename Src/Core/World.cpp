
#include "D3D.h"
#include "World.h"
#include "Xform.h"
#include "Geometry.h"
#include "ParticleSpline.h"

#include "Metaballs.h"
#include "Sprites.h"

// FIXME: Introduce regular STL vectors instead?
#define PIMP_MAX_NUM_WORLD_ELEMENTS 8192
#define PIMP_MAX_NUM_PARTICLEATTRACTORS 256
#define PIMP_MAX_NUM_TEXTURES 256
#define PIMP_MAX_NUM_SCENES 256
#define PIMP_MAX_NUM_CAMERADIRECTIONSHOTS 256
#define PIMP_MAX_NUM_MATERIALS 256

namespace Pimp 
{
	World::World()
		: elements(PIMP_MAX_NUM_WORLD_ELEMENTS),
		useCameraDirector(false),
		directionCameras(PIMP_MAX_NUM_CAMERADIRECTIONSHOTS),
		scenes(PIMP_MAX_NUM_SCENES),
		materials(PIMP_MAX_NUM_MATERIALS),
		currentUserCamera(NULL),
		currentTime(0),
		prevMotionBlurTime(9999999.0),
		particleAttractors(PIMP_MAX_NUM_PARTICLEATTRACTORS),
		textures(PIMP_MAX_NUM_TEXTURES),
		motionBlurAmount(0.0f),
		motionBlurAmountCurve(NULL)
	{
		rootNode = new Xform(this);
		elements.Add(rootNode);


		cameraDirection = new AnimCurve(this);
		cameraDirection->SetInterpolationType(AnimCurve::InterpolationType_Hold);
		cameraDirection->SetKeysPtr(new FixedSizeList<AnimCurve::Pair>(PIMP_MAX_NUM_CAMERADIRECTIONSHOTS));
		elements.Add(cameraDirection);

		sceneDirection = new AnimCurve(this);
		sceneDirection->SetInterpolationType(AnimCurve::InterpolationType_Hold);
		sceneDirection->SetKeysPtr(new FixedSizeList<AnimCurve::Pair>(PIMP_MAX_NUM_SCENES));
		elements.Add(sceneDirection);

		currentSceneIndex = -1;
		currentCamera = NULL;

		postProcess = new PostProcess();

		screenQuadVertexBuffer = new ScreenQuadVertexBuffer(postProcess->GetEffectPassBloomGather());
		screenQuadVertexBuffer->Bind();
	}

	World::~World()
	{
		for (int i=0; i<elements.Size(); ++i)
		{
			delete elements[i];
		}
		elements.Clear();

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

		// Update camera direction
		//View previousView = currentView;
		 

		if (useCameraDirector)
		{
			int currentShotIndex = (int)floorf(cameraDirection->GetCurrentValue());

			if (currentShotIndex >= 0 && currentShotIndex < directionCameras.Size())
			{
				currentCamera = directionCameras[currentShotIndex];
			}
			else
			{
				currentCamera = NULL;
			}
		}
		else
		{
			currentCamera = currentUserCamera;
		}

		//currentSceneIndex = (int)floorf(sceneDirection->GetCurrentValue());

		//if (!scenes.IsValidIndex(currentSceneIndex))
		//	currentSceneIndex = -1;


		float timeSincePrevMotionBlur = currentTime - prevMotionBlurTime;

		if (/*previousView != currentView || */timeSincePrevMotionBlur <= 0.0f)
		{
			// Reset motion blurring since we're not moving forward on the timeline -or-
			// we just switched cameras
			postProcess->SetMotionBlurFrameWeight(1.0f);
		}
		else
		{
			// Calculate weight of current frame. Previous frame will have a weight of 1 - currentframeweight. Note
			// that we have to use pow() since it's an exponential function. The motionBlurAmount that is specified 
			// is an amount when our framerate is exactly 60hz.

			float blurAmount = (motionBlurAmountCurve ? motionBlurAmountCurve->GetCurrentValue() : motionBlurAmount);

			float currentFrameWeight = (blurAmount > 0) ? (1.0f - powf(blurAmount, timeSincePrevMotionBlur*60.0f)) : 1.0f;

			postProcess->SetMotionBlurFrameWeight(currentFrameWeight);
		}

		prevMotionBlurTime = currentTime;
	}


	void World::Render(Sprites *pSprites, Metaballs *pMetaballs)
	{
		ASSERT(nullptr != pSprites);

		// Clear
		postProcess->Clear(); 

		// Bind render target(s)
		postProcess->BindForRenderScene();

		// Draw background sprite
		pSprites->DrawBackgroundSprite();

		// Disable depth stencil
		gD3D->UseDepthStencil(false);

		// Bind screen quad VB for first pass
		screenQuadVertexBuffer->Bind();

		// Bind camera
		if (currentCamera != NULL)
			currentCamera->Bind();

		// Render our scene to a single sceneColor FP16 RT
		if (scenes.IsValidIndex(currentSceneIndex) && 
			scenes[currentSceneIndex] != NULL)
		{
			scenes[currentSceneIndex]->Render(currentCamera);
		}

		// Draw metaballs?
		if (nullptr != pMetaballs)
		{
			// Clear & enable depth stencil
			gD3D->UseDepthStencil(true);
			gD3D->ClearDepthStencil();

			pMetaballs->Draw(currentCamera);

			// Disable depth stencil
			gD3D->UseDepthStencil(false);
		}

		// Bind screen quad VB for RenderPostProcess()
		screenQuadVertexBuffer->Bind();

		// Draw posteffects
		postProcess->RenderPostProcess();

		// ** At this point, the back buffer will be bound **

		// Flush (draw & clear queue) the sprites
		pSprites->FlushSprites();

		// Ensure blend mode is none for next frame
		gD3D->SetBlendMode(D3D::BM_None);

		gD3D->Flip();
	}


	void World::SetCurrentUserCamera(Camera* camera)
	{
		currentUserCamera = camera;
	}

	void World::SetCurrentSceneIndex(int index)
	{
		currentSceneIndex = index;
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

	void World::InitAllBalls()
	{
#ifdef _DEBUG
		for (int i=0; i<elements.Size(); ++i)
			if (elements[i]->GetType() == ET_Balls)
				((Balls*)elements[i])->CalculateCachedBallAnims();
#endif
	}

	void World::SetMotionBlurAmount( float amount )
	{
		motionBlurAmount = amount;
	}



}