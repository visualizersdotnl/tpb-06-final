
#include "Platform.h"
#include <xmmintrin.h>
#include "D3D.h"
// #include "Texture2D.h"

namespace Pimp
{
	Texel Texel::black = { 1, 0, 0, 0 };

	Texture2D::Texture2D(const std::string& name, int width, int height, ID3D11Texture2D* texture, ID3D11ShaderResourceView* view)
		: Texture(name, width, height, view), texture(texture)
	{
	}

	Texture2D::~Texture2D()
	{
		SAFE_RELEASE(texture);
	}

	void Texture2D::DownSampleTo8Bit(void* pDest, Texel* pSrc, int numPixels)
	{
		const _MM_ALIGN16 float thresholdLow[4] = { 0, 0, 0, 0 };  
		const _MM_ALIGN16 float thresholdHigh[4] = { 255.0f, 255.0f, 255.0f, 255.0f };

		const __m128 thresholdLow128 = _mm_load_ps(thresholdLow);
		const __m128 thresholdHigh128 = _mm_load_ps(thresholdHigh);

		const int initialRoundMode = _MM_GET_ROUNDING_MODE();
		_MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST);

		unsigned int* pDest32 = reinterpret_cast<unsigned int*>(pDest);

		while (numPixels-- > 0)
		{
			__m128 srcPixel = _mm_load_ps(reinterpret_cast<float*>(pSrc));

			// scale from [0,1] to [0,255]
			srcPixel = _mm_mul_ps(srcPixel, thresholdHigh128);

			// clamp to [0,255]
			srcPixel = _mm_max_ps(thresholdLow128, srcPixel);
			srcPixel = _mm_min_ps(thresholdHigh128, srcPixel);

			// int. rounding
			__m128i srcPixelInt = _mm_cvtps_epi32(srcPixel);

			// pack 2 sets of 4 32-bit components back to 16-bit (saturated)
			__m128i twice = _mm_packs_epi32( srcPixelInt, srcPixelInt );

			union { __m128i quadPacked; unsigned int quad[4]; };

			// pack 2 sets back to 8-bit (saturated)
			quadPacked = _mm_packus_epi16(twice, twice);

			// quad is stored like this:
			// r0 [bits   0..7]: A
			// r1 [bits  8..15]: R
			// r2 [bits 16..23]: G
			// r3 [bits 24..31]: B

			*(pDest32++) = (quad[0] >> 8) | ((quad[0] & 0xff) << 24);	
			++pSrc;
		}

		_MM_SET_ROUNDING_MODE(initialRoundMode);
	}


	void Texture2D::UploadTexels(Texel* sourceTexels)
	{
		D3D11_MAPPED_SUBRESOURCE mappedTex;
		D3D_VERIFY(gD3D->GetContext()->Map(texture, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_DISCARD, 0, &mappedTex));
		{
			unsigned char* destTexels = reinterpret_cast<unsigned char*>(mappedTex.pData);
			ASSERT(mappedTex.RowPitch == GetWidth()*4); // Assumption: no padding.

			DownSampleTo8Bit(destTexels, sourceTexels,  GetWidth()*GetHeight());
		}
		gD3D->GetContext()->Unmap(texture, D3D11CalcSubresource(0, 0, 1));
	}

	void Texture2D::UploadTexels(unsigned char* sourceTexels)
	{
		D3D11_MAPPED_SUBRESOURCE mappedTex;
		D3D_VERIFY(gD3D->GetContext()->Map(texture, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_DISCARD, 0, &mappedTex));
		{
			unsigned char* destTexels = reinterpret_cast<unsigned char*>(mappedTex.pData);
			for (int iY = 0; iY < GetHeight(); ++iY)
				memcpy(destTexels + iY*mappedTex.RowPitch, sourceTexels + iY*GetWidth()*4, GetWidth()*4);
		}
		gD3D->GetContext()->Unmap(texture, D3D11CalcSubresource(0, 0, 1));
	}
}
