
#include "Platform.h"
#include <xmmintrin.h>
#include <emmintrin.h>
#include "D3D.h"
// #include "Texture3D.h"

namespace Pimp
{
	Texture3D::Texture3D(const std::string& name, int width, int height, int depth, ID3D10Texture3D* texture, ID3D10ShaderResourceView* view, ID3D10RenderTargetView** sliceRenderTargetViews)
		: Texture(name, width, height, view), texture(texture), sliceRenderTargetViews(sliceRenderTargetViews), depth(depth)
	{
	}

	Texture3D::~Texture3D()
	{
		SAFE_RELEASE(texture);

		for (int iSlice = 0; iSlice < depth; ++iSlice)
			SAFE_RELEASE(sliceRenderTargetViews[iSlice]);

		delete[] sliceRenderTargetViews;
	}

	void Texture3D::UploadTexels(float* sourceTexels)
	{
		D3D10_MAPPED_TEXTURE3D mappedTex;
		D3D_VERIFY(texture->Map(D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex));
		{
			unsigned char* destTexels = reinterpret_cast<unsigned char*>(mappedTex.pData);
		
			// Assumption: no padding.
			ASSERT(mappedTex.RowPitch == GetWidth()*4);
			ASSERT(mappedTex.DepthPitch == GetWidth()*GetHeight()*4);
	
			memcpy(destTexels, sourceTexels, GetWidth()*GetHeight()*GetDepth()*4);
		}
		texture->Unmap( D3D10CalcSubresource(0, 0, 1) );
	}
}
