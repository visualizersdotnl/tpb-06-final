
// This is a simple interface to request (or "add") assets and load them in single centralized calls.
// All final pointers are stored locally and can be ditched at once by calling Release().
// Saves a lot of code duplication i.e. typing.

// Resources are added to the global World instance automatically!

// Typical usage pattern:
// - Add any number of resources.
// - Call StartLoading() and check it's result, SetLastError() will have been set in case anything failed.
// - Do stuff?
// - Call FinishLoading() and check it's result, SetLastError() will have been set in case anything failed.
// - On World destruction, call Release().

#pragma once

// #include <string>
// #include <Core/Core.h>

// Exposed for loading bar purposes.
Pimp::Texture2D *LoadPNG(const std::string &path, bool alphaPreMul, bool gammaCorrect);

namespace Assets
{
	void SetRoot(const std::string &root);

	void AddMaterial(const std::string &path, Pimp::Material **ppMaterial);
	void AddTexture2D(const std::string &path, Pimp::Texture2D **ppTexture2D, bool alphaPreMul = false);

	// Completes all blocking (for now) I/O and kicks material compilation jobs.
	bool StartLoading();
	
	// Waits for all threads to be finished.
	// Only after this call are all resources gauranteed to be valid.
	bool FinishLoading();

	void Release();

#if defined(_DEBUG) || defined(_DESIGN)
	// A little hack to replace a material pointer for a new one, used by AutoShaderReload.
	void ReplaceMaterial(Pimp::Material *pOld, Pimp::Material *pNew);
#endif
}
