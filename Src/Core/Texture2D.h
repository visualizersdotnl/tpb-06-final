
#pragma once

#include "Texture.h"

namespace Pimp
{
	class Texture2D : public Texture
	{
	private:
		ID3D11Texture2D* texture;

	public:
		Texture2D(
			const std::string& name, 
			int width, int height, 
			ID3D11Texture2D* texture, ID3D11ShaderResourceView* view);
		
		~Texture2D();

		// Upload texel data from 8-bit RGBA image of exactly the same dimensions.
		void UploadTexels(unsigned char* sourceTexels);
	};
}
