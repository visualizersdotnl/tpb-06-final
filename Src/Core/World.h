#pragma once

#include "Settings.h"
#include "Element.h"
#include "Material.h"
#include "PostProcess.h"
#include "ScreenQuadVertexBuffer.h"

// Nodes
#include "Scene.h"
#include "Camera.h"

namespace Pimp 
{
	// Non-node drawables
	class Sprites;
	class Metaballs;

	class World
	{
	private:
		Node* rootNode;

		Camera* currentCamera;
		int currentSceneIndex;

		// Anything that's Tick()-able.
		// Owned and deleted right here.
		std::vector<Element*> elements;

		std::vector<Texture*>  textures;  // Textures (no element, no ownership).
		std::vector<Material*> materials; // Materials (no element, no ownership).
		std::vector<Scene*>    scenes;    // List of scenes. Each scene is also included in the elements list.

		PostProcess* postProcess;
		ScreenQuadVertexBuffer* screenQuadVertexBuffer;

		float currentTime;

		float motionBlurAmount;
		float prevMotionBlurTime;

	public:
		World();
		virtual ~World();

		std::vector<Element*>&  GetElements()  { return elements;  }
		std::vector<Texture*>&  GetTextures()  { return textures;  }
		std::vector<Material*>& GetMaterials() { return materials; }
		std::vector<Scene*>&    GetScenes()    { return scenes;    }

		Node* GetRootNode() const 
		{ 
			return rootNode; 
		}

		void SetCamera(Camera* camera) { currentCamera = camera; }
		Camera* GetCamera() const      { return currentCamera;   }

		PostProcess* GetPostProcess() const
		{
			return postProcess;
		}

		void Tick(float deltaTime);
		void Render(Pimp::Sprites &, Pimp::Metaballs *);

		void SetCurrentSceneIndex(int index) 
		{ 
			ASSERT(-1 == currentSceneIndex || currentSceneIndex < scenes.size());
			currentSceneIndex = index; 
		}

		int GetCurrentSceneIndex() const 
		{ 
			return currentSceneIndex; 
		}

		void ForceSetTime(float time);

		float GetCurrentTime() const
		{
			return currentTime;
		}

		void UpdateAllMaterialParameters();

		void SetMotionBlurAmount(float amount);

		static void StaticAddChildToParent(Node* nodeChild, Node* nodeParent);
		static void StaticRemoveChildFromParent(Node* nodeChild, Node* nodeParent);
	};
}
