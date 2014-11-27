
#pragma once

#include "Texture.h"

namespace Pimp
{
	class Texture3D : public Texture
	{
	private:
		int depth;
		ID3D11Texture3D* texture;
		ID3D11RenderTargetView** sliceRenderTargetViews;

	public:
		Texture3D(
			const std::string& name, 
			int width, int height, int depth, 
			ID3D11Texture3D* texture, ID3D11ShaderResourceView* view, ID3D11RenderTargetView** sliceRenderTargetViews);

		~Texture3D();

		// Upload texel data from a 32-bit float source buffer.
		void UploadTexels(float* sourceTexels);

		ID3D11RenderTargetView* GetRenderTargetView(int sliceIndex) const
		{ 
			ASSERT(sliceIndex >= 0 && sliceIndex < GetDepth());
			return sliceRenderTargetViews[sliceIndex]; 
		}

		int GetDepth() const { return depth; }
	};
}