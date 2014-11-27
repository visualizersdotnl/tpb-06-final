
//
// Scene (or 'part' in classic demoscene lingo) base class.
// Primarily intended to manage all resource requests, objects and world manipulation for a single scene.
//
// IMPORTANT: all objects added as an element to the world are destroyed by the world itself!
//

class Scene
{
public:
	Scene() :
		m_pScene(nullptr),
		m_sceneIdx(-1)
	{
	}

	virtual ~Scene() {}

	// Called after Rocket is fired up. Request your private tracks here.
	// ^^ Don't forget there's a global amount of tracks like g_fxTimer!
	virtual void ReqRocketTracks() = 0;

	// Called prior to loading process: request assets only.
	virtual void ReqAssets() = 0;           

	// Create and bind world elements. 
	// Remember: asset loader takes care of it for assets.
	virtual void BindToWorld() = 0;        

	// Called from Demo::Tick().
	virtual void Tick(double row) = 0;

protected:
	// Assuming that each part has at least one scene shader.
	Pimp::Scene *m_pScene;
	int m_sceneIdx;

	// Call this in BindToWorld() to bind the shader to the main scene.
	void BindSceneMaterial(Pimp::Material *pMat)
	{
		if (nullptr == m_pScene)
		{
			m_pScene = new Pimp::Scene(s_pWorld, *pMat);
			s_pWorld->GetScenes().push_back(m_pScene);
			s_pWorld->GetElements().push_back(m_pScene);
			m_sceneIdx = int(s_pWorld->GetScenes().size()-1);	
		}
	}

	// Helper to bind a material parameter to Xform (regular or inverse) and add it to the world.
	static Pimp::MaterialParameter* AddMaterialParamWithXform(const char* name, bool inverse)
	{
		Pimp::Xform* xform = new Pimp::Xform(s_pWorld);
		
		Pimp::MaterialParameter* matParam = new Pimp::MaterialParameter(s_pWorld);
		s_pWorld->GetElements().push_back(matParam);
		
		if (true ==  inverse)
			matParam->SetValueType(Pimp::MaterialParameter::VT_NodeXformInv);
		else
			matParam->SetValueType(Pimp::MaterialParameter::VT_NodeXform);

		matParam->SetName(name);

		Pimp::World::StaticAddChildToParent(xform, s_pWorld->GetRootNode());
		Pimp::World::StaticAddChildToParent(matParam, xform);

		// @plek: As the transform node becomes a child of the paramater, the node destructor should
		//        eventually delete it. Confirm this some day.

		return matParam;
	}

	// And this one on top of Tick() to activate said scene.
	void SetMainSceneAndCamera(Pimp::Camera *pCamera) 
	{ 
		s_pWorld->SetCamera(pCamera);
		s_pWorld->SetCurrentSceneIndex(m_sceneIdx); 
	}
};

static std::vector<Scene *> s_scenes;
