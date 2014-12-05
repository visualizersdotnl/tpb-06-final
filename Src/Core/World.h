
#pragma once

#include "Settings.h"
#include "Element.h"
#include "Material.h"
#include "PostProcess.h"
#include "ScreenQuad.h"

// Nodes
#include "Scene.h"
#include "Camera.h"

namespace Pimp 
{
	// Non-node drawables
	class Sprites;
	class Metaballs;

	class World : public boost::noncopyable
	{
	private:
		Node* rootNode;

		// Anything that's Tick()-able.
		// Owned and deleted right here.
		std::vector<Element*> elements;

		std::vector<Texture*>  textures;  // Textures (no element, no ownership).
		std::vector<Material*> materials; // Materials (no element, no ownership).
		std::vector<Scene*>    scenes;    // List of scenes. Each scene is also included in the elements list.

		PostProcess postProcess;

		Camera* currentCamera;
		int currentSceneIndex;

		float currentTime;
		float motionBlurAmount;
		float prevMotionBlurTime;

	public:
		World();
		virtual ~World();

		void Tick(float deltaTime);
		void UpdateAllMaterialParameters();
		void Render(Pimp::Sprites &, Pimp::Metaballs *);

		Node* GetRootNode() const 
		{ 
			return rootNode; 
		}

		std::vector<Element*>&  GetElements()  { return elements;  }
		std::vector<Texture*>&  GetTextures()  { return textures;  }
		std::vector<Material*>& GetMaterials() { return materials; }
		std::vector<Scene*>&    GetScenes()    { return scenes;    }

		PostProcess& GetPostProcess()
		{
			return postProcess;
		}

		void SetCamera(Camera* camera) { currentCamera = camera; }
		Camera* GetCamera() const      { return currentCamera;   }

		void SetCurrentSceneIndex(int index) 
		{ 
			ASSERT(-1 == currentSceneIndex || currentSceneIndex < scenes.size());
			currentSceneIndex = index; 
		}

		int GetCurrentSceneIndex() const 
		{ 
			return currentSceneIndex; 
		}

		void ForceSetTime(float time)
		{ 
			currentTime = time; 
		}

		void SetMotionBlurAmount(float amount) 
		{ 
			motionBlurAmount = amount; 
		}
		
		static void StaticAddChildToParent(Node* nodeChild, Node* nodeParent);
		static void StaticRemoveChildFromParent(Node* nodeChild, Node* nodeParent);
	};
}
