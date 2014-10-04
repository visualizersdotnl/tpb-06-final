#include "D3D.h"
#include "Texture.h"



namespace Pimp 
{
	Texture::Texture(const std::string& name, int width, int height, ID3D10ShaderResourceView* view)
		:  name(name), view(view), width(width), height(height)
	{
	}

	Texture::~Texture()
	{
		if (view != NULL)
		{
			view->Release();
			view = NULL;
		}
	}
}