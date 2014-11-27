#pragma once

#include "D3D.h"

namespace Pimp
{
	class DepthStencil
	{
	public:
		DepthStencil(ID3D11Texture2D* texture, ID3D11DepthStencilView* view);
		~DepthStencil();

		ID3D11DepthStencilView* GetDepthStencilView() { return view; }

	private:
		ID3D11Texture2D* texture;
		ID3D11DepthStencilView* view;
	};
}
