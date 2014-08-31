#include "Texture.h"
#include "D3D.h"



namespace Pimp 
{
	Texture::Texture(const std::string& name, int sizePixels,ID3D10ShaderResourceView* view)
		:  name(name), view(view), sizePixels(sizePixels)
	{
	}

#ifdef _DEBUG
	Texture::~Texture()
	{
		if (view != NULL)
		{
			view->Release();
			view = NULL;
		}
	}
#endif

}