
#pragma once

#include "Texture.h"

namespace Pimp
{
	class Texture3D : public Texture
	{
	private:
		int depth;
		ID3D10Texture3D* texture;
		ID3D10RenderTargetView** sliceRenderTargetViews; // count: size, one for each slice

	public:
		Texture3D(const std::string& name, int width, int height, int depth, ID3D10Texture3D* texture, ID3D10ShaderResourceView* view, ID3D10RenderTargetView** sliceRenderTargetViews);
		virtual ~Texture3D();

		// Upload texel data from a 32bit-fp source buffer.
		void UploadTexels(float* sourceTexels);


		ID3D10RenderTargetView* GetRenderTargetView(int sliceIndex) const
		{ 
			ASSERT(sliceIndex >= 0 && sliceIndex < GetDepth());

			return sliceRenderTargetViews[sliceIndex]; 
		}


		int GetDepth() const
		{
			return depth;
		}
	};
}