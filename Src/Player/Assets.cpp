
#include <Core/Platform.h>
#include <Core/Core.h>
#include <Shared/fileutils.h>
#include <LodePNG/lodepng.h>
// #include "SceneTools.h"
#include "Assets.h"
#include "Settings.h"
#include "MaterialCompiler.h"
#include "SetLastError.h"
#include "Content/Demo.h"

//
// PNG loader.
//

static Pimp::Texture2D *LoadPNG(const std::string &path, bool alphaPreMul, bool gammaCorrect)
{
	std::string ID = GetFilenameWithoutExtFromPath(path);
	std::transform(ID.begin(), ID.end(), ID.begin(), ::tolower);

	std::vector<unsigned char> pixels;
	unsigned int /* size_t */ width, height;

	const unsigned int error = lodepng::decode(pixels, width, height, path);
	if (0 != error)
	{
		SetLastError("Can't load PNG file from: " + path + "\n" + "Reason: " + lodepng_error_text(error));
		return nullptr;
	}

	ASSERT(false == pixels.empty());

	if (true == alphaPreMul)
	{
		unsigned int *pPixels = reinterpret_cast<unsigned int *>(&pixels[0]);
		for (unsigned int iPixel = 0; iPixel < width*height; ++iPixel)
		{
			const unsigned int pixel = *pPixels;
			
			const unsigned int alpha = pixel >> 24;
			unsigned int red = (pixel >> 16) & 0xff;
			unsigned int green = (pixel >> 8) & 0xff;
			unsigned int blue = pixel & 0xff;
			
			red = (red*alpha)>>8;
			green = (green*alpha)>>8;
			blue = (blue*alpha)>>8;
			
			*pPixels++ = (alpha<<24)|(red<<16)|(green<<8)|blue;
		}
	}

	Pimp::Texture2D *pTexture = Pimp::gD3D->CreateTexture2D(ID, width, height, gammaCorrect);
	pTexture->UploadTexels(&pixels[0]);
	return pTexture;
}

//
// Loading system.
//

static MaterialCompiler s_fxCompiler;

class MaterialRequest // Basically an FX shader.
{
public:
	MaterialRequest(const std::string &path, Pimp::Material **ppDest) :
		path(path),
		ppDest(ppDest),
		source(nullptr),
		bytecode(nullptr),
		errorMsg("Unknown") {}

	~MaterialRequest() 
	{ 
		delete[] source;
		delete[] bytecode; 
	}

	const std::string path;
	Pimp::Material **ppDest;

	unsigned char *source, *bytecode;
	int sourceSize, bytecodeSize;

	std::string errorMsg;
};

class Texture2DRequest
{
public:
	Texture2DRequest(const std::string &path, bool alphaPreMul, Pimp::Texture2D **ppDest) :
		path(path),
		alphaPreMul(alphaPreMul),
		gammaCorrect(true),
		ppDest(ppDest) {}

	const std::string path;
	const bool alphaPreMul;
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
		s_materialReqs.push_back(MaterialRequest(s_root + path, ppMaterial));
	}

	static bool LoadMaterials()
	{
		// FIXME: Set correct loading bar size.
		// ...

		if (false == PIMPPLAYER_RUN_FROM_SHADER_BINARIES)
		{
			// Compile from source.
			for (MaterialRequest &request : s_materialReqs)
			{
				if (false == ReadFileContent(request.path, &request.source, &request.sourceSize))
				{
					SetLastError("Can not load shader source: " + request.path);
					return false;
				}

				s_fxCompiler.StartJob(
					request.path,
					request.source, request.sourceSize, 
					&request.bytecode, &request.bytecodeSize,
					request.errorMsg);
			}
		}
		else
		{
			// Run from binaries.
			for (MaterialRequest &request : s_materialReqs)
			{
				if (false == ReadFileContent(request.path + "b", &request.bytecode, &request.bytecodeSize))
				{
					SetLastError("Can not load shader binary: " + request.path + "b");
					return false;
				}
			}
		}

		return true;
	}

	void AddTexture2D(const std::string &path, Pimp::Texture2D **ppTexture2D, bool alphaPreMul /* = false */ )
	{
		s_textureReqs.push_back(Texture2DRequest(s_root + path, alphaPreMul, ppTexture2D));
	}

	static bool LoadTextures()
	{
		for (auto &request : s_textureReqs)
		{
			Pimp::Texture2D *pTex = LoadPNG(request.path, request.alphaPreMul, request.gammaCorrect);
			if (nullptr == pTex)
				return false;

			if (request.ppDest != NULL)
				*request.ppDest = pTex;
			s_textures.push_back(pTex);

			Demo::GetWorld()->GetTextures().push_back(pTex);
		}

		return true;
	}
	
	bool StartLoading()
	{
		// Potentially kicks off a number of threads.
		if (false == LoadMaterials())
			return false;

		// More or less pure disk I/O.
		if (false == LoadTextures())
			return false;

		return true;
	}

	bool FinishLoading()
	{
		s_fxCompiler.WaitForCompletion();
		
		// Finish up materials, bytecode is now ready!
		for (auto &request : s_materialReqs)
		{
			if (false == PIMPPLAYER_RUN_FROM_SHADER_BINARIES)
			{
				if (-1 == request.bytecodeSize)
				{
					std::stringstream error;
					error << "Failed to compile shader: ";
					error << request.errorMsg;
					SetLastError(error.str());
					return false;
				}
	
				// In dev. mode, dump shader binary to disk.
				VERIFY(true == WriteFileContent(request.path + "b", request.bytecode, request.bytecodeSize));
			}

			Pimp::Material *pMat = new Pimp::Material(Demo::GetWorld(), request.bytecode, request.bytecodeSize, request.path);

			*request.ppDest = pMat;
			s_materials.push_back(pMat);
			Demo::GetWorld()->GetMaterials().push_back(pMat);
		}

		// Ditch requests.
		s_materialReqs.clear();
		s_textureReqs.clear();

		return true;
	}

	void Release()
	{
		for (auto *pMat : s_materials)
			delete pMat;
		
		for (auto *pTex2D : s_textures)
			delete pTex2D;

		s_materials.clear();
		s_textures.clear();
	}

#if defined(_DEBUG) || defined(_DESIGN)

	void ReplaceMaterial(Pimp::Material *pOld, Pimp::Material *pNew)
	{
		auto iMat = std::find(s_materials.begin(), s_materials.end(), pOld);
		if (s_materials.end() != iMat)
		{
			*iMat = pNew;
		}
	}

#endif

}
