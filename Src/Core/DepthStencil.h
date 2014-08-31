#pragma once

#include "D3D.h"

namespace Pimp
{
	class DepthStencil
	{
	public:
		DepthStencil(ID3D10Texture2D* texture, ID3D10DepthStencilView* view);
		~DepthStencil();

		ID3D10DepthStencilView* GetDepthStencilView() { return view; }

	private:
		ID3D10Texture2D* texture;
		ID3D10DepthStencilView* view;
	};
}


