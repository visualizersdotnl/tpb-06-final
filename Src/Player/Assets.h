
// This is a simple interface to request (or "add") assets and load them in single centralized calls.
// All final pointers are stored locally and can be ditched at once by calling Release().
// Saves a lot of code duplication i.e. typing.

// Typical usage pattern:
// - Add any number of resources.
// - Call load functions and check their result, SetLastError() will have been set in case anything failed.
// - Finally FinishLoading() for any multithreaded loading (e.g. shader compilation) to finish up completely.
// - On World destruction, call Release().

#pragma once

// #include <string>
// #include <Core/Core.h>

namespace Assets
{
	void SetRoot(const std::string &root);

	void AddMaterial(const std::string &path, Pimp::Material **ppMaterial);
	bool LoadMaterials();

	void AddTexture2D(const std::string &path, bool gammaCorrect, Pimp::Texture2D **ppTexture2D);
	bool LoadTextures();
	
	// Waits for all threads to be finished.
	// Only after this call are all resources gauranteed to be valid.
	void FinishLoading();

	void Release();
}
