#pragma once

#include <d3d10_1.h>
#include "Texture.h"
#include <shared/shared.h>

namespace Pimp
{
	class Texture3D : public Texture
	{
	private:
		int size3; // size^3
		ID3D10Texture3D* texture;
		ID3D10RenderTargetView** sliceRenderTargetViews; // count: size, one for each slice

	public:
		Texture3D(const std::string& name, int sizePixels, ID3D10Texture3D* texture, ID3D10ShaderResourceView* view, ID3D10RenderTargetView** sliceRenderTargetViews);

#ifdef _DEBUG
		virtual ~Texture3D();
#endif

		// Upload texel data from a 32bit-fp source buffer.
		void UploadTexels(float* sourceTexels);

		ID3D10RenderTargetView* GetRenderTargetView(int sliceIndex) const
		{ 
			ASSERT(sliceIndex >= 0 && sliceIndex < GetSizePixels());

			return sliceRenderTargetViews[sliceIndex]; 
		}
	};
}