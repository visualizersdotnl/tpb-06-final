
#pragma once

#include "Texture.h"

namespace Pimp
{
	struct Texel
	{
		union
		{
			struct 
			{
				float a, b, g, r;
			};
			float component[4];
		};

		static Texel black;
	};

	class Texture2D : public Texture
	{
	private:
		ID3D11Texture2D* texture;
		void DownSampleTo8Bit(void* pDest, Texel* pSrc, int numPixels);

	public:
		Texture2D(
			const std::string& name, 
			int width, int height, 
			ID3D11Texture2D* texture, ID3D11ShaderResourceView* view);
		
		~Texture2D();

		// Upload texels from 32-bit float RGBA image of exactly the same dimensions.
		// Quantizes to 8-bit RGBA.
		void UploadTexels(Texel* sourceTexels);

		// Upload texel data from 8-bit RGBA image of exactly the same dimensions.
		void UploadTexels(unsigned char* sourceTexels);
	};
}
