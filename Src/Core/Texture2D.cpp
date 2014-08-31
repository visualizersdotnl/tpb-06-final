#include "Texture2D.h"
#include "D3D.h"
#include <xmmintrin.h>
#include <emmintrin.h>


namespace Pimp
{
	Texel Texel::black = { 1, 0, 0, 0 };

	Texture2D::Texture2D(const std::string& name, int sizePixels, ID3D10Texture2D* texture, ID3D10ShaderResourceView* view)
		: Texture(name, sizePixels, view), texture(texture)
	{

	}

#ifdef _DEBUG
	Texture2D::~Texture2D()
	{
		if (texture != NULL)
		{
			texture->Release();
			texture = NULL;
		}
	}
#endif

	void Texture2D::DownSampleTo8Bit(void* dest, Texel* source, int numPixels)
	{
		const _MM_ALIGN16 float thresholdLow[4] = { 0, 0, 0, 0 };  
		const _MM_ALIGN16 float thresholdHigh[4] = { 255.0f, 255.0f, 255.0f, 255.0f };

		__m128 thresholdLow128 = _mm_load_ps(thresholdLow);
		__m128 thresholdHigh128 = _mm_load_ps(thresholdHigh);

		int initialRoundMode = _MM_GET_ROUNDING_MODE();
		_MM_SET_ROUNDING_MODE( _MM_ROUND_NEAREST );

		unsigned int* dest32 = (unsigned int*)dest;

		while (numPixels-- > 0)
		{
			__m128 srcPixel = _mm_load_ps((float*)source);

			// scale from [0,1] to [0,255]
			srcPixel = _mm_mul_ps(srcPixel, thresholdHigh128);

			// clamp to [0,255]
			srcPixel = _mm_max_ps(thresholdLow128, srcPixel);
			srcPixel = _mm_min_ps(thresholdHigh128, srcPixel);

			// round to nearest int (using sse2)
			__m128i srcPixelInt = _mm_cvtps_epi32(srcPixel);

			// pack 2 sets of 4 32bit ints together into 8 16 bit ints (also saturate)
			__m128i twice = _mm_packs_epi32( srcPixelInt, srcPixelInt );

			union { __m128i quadPacked; unsigned int quad[4]; };

			// pack 2 sets of 8 16bit ints into 16 8bit ints (also saturate)
			quadPacked = _mm_packus_epi16(twice, twice);

			// quad is stored like this:
			// r0 [bits  0..7]: first pixel, alpha
			// r1 [bits 8..15]: first pixel, R
			// r2 [bits 16..23]: first pixel, G
			// r3 [bits 24..31]: first pixel, B

			*(dest32++) = (quad[0] >> 8) | ((quad[0] & 0xff) << 24);	
			++source;
		}

		_MM_SET_ROUNDING_MODE( initialRoundMode );
	}


	void Texture2D::UploadTexels(Texel* sourceTexels)
	{
		D3D10_MAPPED_TEXTURE2D mappedTex;
		texture->Map( D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex );

		unsigned char* destTexels = (unsigned char*)mappedTex.pData;
		ASSERT(mappedTex.RowPitch == GetSizePixels()*4); // We're assuming this...

		DownSampleTo8Bit(destTexels, sourceTexels, GetSizePixels()*GetSizePixels());

		//#ifdef _DEBUG
		//		char s[256];
		//		sprintf_s(s, 256, "c:\\temp\\tex\\tex%d.tga",id);
		//
		//		StoreTGAImageToFile(s, sizePixels, sizePixels, destTexels);
		//#endif

		texture->Unmap( D3D10CalcSubresource(0, 0, 1) );
	}


	void Texture2D::UploadTexels(unsigned char* sourceTexels)
	{
		D3D10_MAPPED_TEXTURE2D mappedTex;
		texture->Map( D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex );

		unsigned char* destTexels = (unsigned char*)mappedTex.pData;
		ASSERT(mappedTex.RowPitch == GetSizePixels()*4); // We're assuming this...

		memcpy(destTexels, sourceTexels, GetSizePixels()*GetSizePixels()*sizeof(DWORD));

		texture->Unmap( D3D10CalcSubresource(0, 0, 1) );
	}
}


