#pragma once

#include <d3d10_1.h>
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
		ID3D10Texture2D* texture;
		void DownSampleTo8Bit(void* dest, Texel* source, int numPixels);

	public:
		Texture2D(const std::string& name, int sizePixels, ID3D10Texture2D* texture, ID3D10ShaderResourceView* view);

#ifdef _DEBUG
		virtual ~Texture2D();
#endif

		// Upload texel data from a 32bit-fp-per-component RGBA image of exactly the same dimensions.
		// this will downsample it to 8bit-per-component.
		void UploadTexels(Texel* sourceTexels);

		// Upload texel data from a 8bit-per-component RGBA image of exactly the same dimensions.
		void UploadTexels(unsigned char* sourceTexels);
	};
}