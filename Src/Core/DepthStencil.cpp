#include "DepthStencil.h"

namespace Pimp
{

	DepthStencil::DepthStencil(ID3D10Texture2D* texture, ID3D10DepthStencilView* view)
		: texture(texture), view(view)
	{

	}

	DepthStencil::~DepthStencil()
	{
		view->Release();
		texture->Release();
	}

}
