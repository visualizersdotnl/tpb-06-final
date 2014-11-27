
#include "Platform.h"
#include "D3D.h"
// #include "Texture.h"

namespace Pimp 
{
	Texture::Texture(const std::string& name, int width, int height, ID3D11ShaderResourceView* view)
		:  name(name), view(view), width(width), height(height)
	{
	}

	Texture::~Texture()
	{
		SAFE_RELEASE(view);
	}
}
