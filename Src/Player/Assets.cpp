
// #include <string>
#include <vector>
#include <Core/Core.h>
#include <Shared/shared.h>
#include <LodePNG/lodepng.h>
#include "Assets.h"
#include "SceneTools.h"
#include "SetLastError.h"

// Loaders.
//

static Pimp::Texture2D *LoadPNG(const std::string &path, bool gammaCorrect)
{
	const std::string ID = LowerCase(GetFilenameWithoutExtFromPath(path));

	std::vector<unsigned char> pixels;
	unsigned int /* size_t */ width, height;

	const unsigned int error = lodepng::decode(pixels, width, height, path);
	if (0 != error)
	{
		ASSERT(0);
		SetLastError("Can't load PNG file from: " + path + "\n" + "Reason: " + lodepng_error_text(error));
		return nullptr;
	}

	ASSERT(0 != pixels.size());
	Pimp::Texture2D *pTexture = Pimp::gD3D->CreateTexture2D(ID, width, height, gammaCorrect);
	pTexture->UploadTexels(&pixels[0]);
	return pTexture;
}

static bool CompileShader(const std::string &path)
{
	return true;
}

// Hub.
//

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
	Texture2DRequest(const std::string &path, bool gammaCorrect, Pimp::Texture2D **ppDest) :
		path(path),
		gammaCorrect(gammaCorrect),
		ppDest(ppDest) {}

	const std::string path;
	const bool gammaCorrect;
	Pimp::Texture2D **ppDest;
};

static std::vector<MaterialRequest> s_materialReqs;
static std::vector<Pimp::Material *> s_materials;

static std::vector<Texture2DRequest> s_textureReqs;
static std::vector<Pimp::Texture2D *> s_textures;

static std::string s_root = "";

namespace Assets
{
	void SetRoot(const std::string &root)
	{
		s_root = root;
	}

	void AddMaterial(const std::string &path, Pimp::Material **ppMaterial)
	{
		s_materialReqs.push_back(MaterialRequest(path, ppMaterial));
	}

	bool LoadMaterials()
	{
		// Make sure the loading bar is somewhat correct.
		SetNumTotalMaterialCompilationJobs((int) s_materialReqs.size());

		for (MaterialRequest &request : s_materialReqs)
		{
		}

		return true;
	}

	void AddTexture2D(const std::string &path, bool gammaCorrect, Pimp::Texture2D **ppTexture2D)
	{
		s_textureReqs.push_back(Texture2DRequest(path, gammaCorrect, ppTexture2D));
	}

	bool LoadTextures()
	{
		for (Texture2DRequest &request : s_textureReqs)
		{
			Pimp::Texture2D *pTex = LoadPNG(s_root + request.path, request.gammaCorrect);
			if (nullptr == pTex)
				return false;

			*request.ppDest = pTex;
			s_textures.push_back(pTex);
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
