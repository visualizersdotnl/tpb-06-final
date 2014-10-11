#pragma once

#include <Shared/shared.h>

#include "Settings.h"
#include "Element.h"
#include "Node.h"
#include "Camera.h"
#include "Material.h"
#include "AnimCurve.h"
#include "ParticleAttractor.h"
#include "PostProcess.h"
#include "Texture.h"
#include "ScreenQuadVertexBuffer.h"
#include "Scene.h"
#include "Overlay.h"
#include "Sprites.h"

namespace Pimp 
{
	class World
	{
	private:
		FixedSizeList<Element*> elements;
		Node* rootNode;
		FixedSizeList<ParticleAttractor*> particleAttractors;

		Camera* currentUserCamera;

		FixedSizeList<Texture*> textures;
		
		bool useCameraDirector;

		AnimCurve* cameraDirection;	// Defines the index of the camera to use over time
		AnimCurve* sceneDirection; // Defines the index of the scene to render over time

		Camera* currentCamera;
		int currentSceneIndex;

		FixedSizeList<Material*> materials;

		FixedSizeList<Camera*> directionCameras;
		FixedSizeList<Scene*> scenes;	// List of scenes. Each scene is also included in the elements list.
		

		FixedSizeList<Overlay*> overlays;  // List of overlays. Each overlay is also included in the elements list.

		PostProcess* postProcess;
		ScreenQuadVertexBuffer* screenQuadVertexBuffer;

		float currentTime;

		float motionBlurAmount;
		AnimCurve* motionBlurAmountCurve;
		float prevMotionBlurTime;

		//void FillVolume3DTexture();

	public:
		World();
		virtual ~World();

		FixedSizeList<Element*>& GetElements() 
		{
			return elements;
		}

		FixedSizeList<Texture*>& GetTextures() 
		{
			return textures;
		}

		FixedSizeList<Material*>& GetMaterials() 
		{
			return materials;
		}

		FixedSizeList<Scene*>& GetScenes() 
		{
			return scenes;
		}

		FixedSizeList<Overlay*>& GetOverlays()
		{
			return overlays;
		}

		Node* GetRootNode() const 
		{
			return rootNode;
		}

		Camera* GetCurrentCamera() const
		{
			return currentCamera;
		}

		const FixedSizeList<ParticleAttractor*>& GetParticleAttractors() const
		{
			return particleAttractors;
		}

		PostProcess* GetPostProcess() const
		{
			return postProcess;
		}

		void Tick(float deltaTime);
		void Render(Pimp::Sprites *);

		void SetCurrentUserCamera(Camera* camera);
		Camera* GetCurrentUserCamera() const 
		{
			return currentUserCamera;
		}

		void SetCurrentSceneIndex(int index);
		int GetCurrentSceneIndex() const
		{
			return currentSceneIndex;
		}

		void SetUseCameraDirection(bool newValue) { useCameraDirector = newValue; }
		bool GetUseCameraDirection() const { return useCameraDirector; }

		void ForceSetTime(float time);

		float GetCurrentTime() const
		{
			return currentTime;
		}

		void RegisterParticleAttractor(ParticleAttractor* attractor)
		{
			particleAttractors.Add(attractor);
		}

		void UnregisterParticleAttractor(ParticleAttractor* attractor)
		{
			particleAttractors.Remove(attractor);
		}

		FixedSizeList<Camera*>& GetDirectionCameras()
		{
			return directionCameras;
		}

		AnimCurve* GetCameraDirectionAnimCurve()
		{
			return cameraDirection;
		}

		AnimCurve* GetSceneDirectionAnimCurve()
		{
			return sceneDirection;
		}



		void UpdateAllMaterialParameters();


		void SetMotionBlurAmount(float amount);

		void SetMotionBlurAmountCurve(AnimCurve* curve)
		{
			motionBlurAmountCurve = curve;
		}

		void InitAllBalls();

		static void StaticAddChildToParent(Node* nodeChild, Node* nodeParent);
		static void StaticRemoveChildFromParent(Node* nodeChild, Node* nodeParent);
	};
}