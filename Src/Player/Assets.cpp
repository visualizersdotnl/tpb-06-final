
// #include <string>
#include <vector>
#include <Core/Core.h>
#include <Shared/shared.h>
#include "Assets.h"
#include "SceneTools.h"

class MaterialRequest
{
public:
	MaterialRequest(const std::string &path, Pimp::Material **ppDest) :
		path(path),
		ppDest(ppDest),
		pBytecode(nullptr) {}

	~MaterialRequest() { delete[] pBytecode; }

	const std::string path;
	Pimp::Material **ppDest;
	unsigned char *pBytecode;
	size_t codeSize;
};

class Texture2DRequest
{
public:
	Texture2DRequest(const std::string &path, Pimp::Texture2D **ppDest) :
		path(path),
		ppDest(ppDest) {}

	const std::string path;
	Pimp::Texture2D **ppDest;
};

static std::vector<MaterialRequest> s_materialReqs;
static std::vector<Pimp::Material *> s_materials;

static std::vector<Texture2DRequest> s_textureReqs;
static std::vector<Pimp::Texture2D *> s_textures;

namespace Assets
{
	void AddMaterial(const std::string &path, Pimp::Material **ppMaterial)
	{
		s_materialReqs.push_back(MaterialRequest(path, ppMaterial));
	}

	bool LoadMaterials()
	{
		for (MaterialRequest &request : s_materialReqs)
		{
		}

		return true;
	}

	void AddTexture2D(const std::string &path, Pimp::Texture2D **ppTexture2D)
	{
		s_textureReqs.push_back(Texture2DRequest(path, ppTexture2D));
	}

	bool LoadTextures()
	{
		for (Texture2DRequest &request : s_textureReqs)
		{
		}

		return true;
	}
	
	void FinishLoading()
	{
		WaitForMaterialCompilationJobsToFinish();
		
		// Finish up materials, bytecode is now ready!
		for (MaterialRequest &request : s_materialReqs)
		{
		}

		// Ditch requests.
		s_materialReqs.clear();
		s_textureReqs.clear();
	}

	void Release()
	{
		for (Pimp::Material *pMat : s_materials)
			delete pMat;
		
		for (Pimp::Texture2D *pTex2D : s_textures)
			delete pTex2D;

		s_materials.clear();
		s_textures.clear();
	}
}
