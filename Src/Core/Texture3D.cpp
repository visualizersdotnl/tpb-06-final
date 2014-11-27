
#include "Platform.h"
#include <xmmintrin.h>
#include <emmintrin.h>
#include "D3D.h"
// #include "Texture3D.h"

namespace Pimp
{
	Texture3D::Texture3D(const std::string& name, int width, int height, int depth, ID3D11Texture3D* texture, ID3D11ShaderResourceView* view, ID3D11RenderTargetView** sliceRenderTargetViews)
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
		D3D11_MAPPED_SUBRESOURCE mappedTex;
		D3D_VERIFY(gD3D->GetContext()->Map(texture, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_DISCARD, 0, &mappedTex));
		{
			unsigned char* destTexels = reinterpret_cast<unsigned char*>(mappedTex.pData);
		
			// Assumption: no padding.
			ASSERT(mappedTex.RowPitch == GetWidth()*4);
			ASSERT(mappedTex.DepthPitch == GetWidth()*GetHeight()*4);
	
			memcpy(destTexels, sourceTexels, GetWidth()*GetHeight()*GetDepth()*4);
		}
		gD3D->GetContext()->Unmap(texture, D3D11CalcSubresource(0, 0, 1));
	}
}
