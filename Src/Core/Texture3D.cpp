#include "Texture3D.h"
#include "D3D.h"
#include <xmmintrin.h>
#include <emmintrin.h>


namespace Pimp
{
	Texture3D::Texture3D(const std::string& name, int sizePixels, ID3D10Texture3D* texture, ID3D10ShaderResourceView* view, ID3D10RenderTargetView** sliceRenderTargetViews)
		: Texture(name, sizePixels, view), texture(texture), sliceRenderTargetViews(sliceRenderTargetViews)
	{
		size3 = sizePixels*sizePixels*sizePixels;
	}

#ifdef _DEBUG
	Texture3D::~Texture3D()
	{
		if (texture != NULL)
		{
			texture->Release();
			texture = NULL;
		}

		for (int i=0; i<GetSizePixels(); ++i)
		{
			if (sliceRenderTargetViews[i] != NULL)
			{
				sliceRenderTargetViews[i]->Release();
				sliceRenderTargetViews[i] = NULL;
			}
		}

		delete [] sliceRenderTargetViews;
	}
#endif

	void Texture3D::UploadTexels(float* sourceTexels)
	{
		D3D10_MAPPED_TEXTURE3D mappedTex;
		texture->Map( D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex );

		unsigned char* destTexels = (unsigned char*)mappedTex.pData;
		ASSERT(mappedTex.RowPitch == GetSizePixels()*4); // We're assuming this...
		ASSERT(mappedTex.DepthPitch == GetSizePixels()*GetSizePixels()*4); // We're assuming this...

		memcpy(destTexels, sourceTexels, size3*4);

		//#ifdef _DEBUG
		//		char s[256];
		//		sprintf_s(s, 256, "c:\\temp\\tex\\tex%d.tga",id);
		//
		//		StoreTGAImageToFile(s, sizePixels, sizePixels, destTexels);
		//#endif

		texture->Unmap( D3D10CalcSubresource(0, 0, 1) );
	}

}


