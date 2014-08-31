#pragma once

#include <d3d10_1.h>
#include <string>

namespace Pimp
{
	class Texture
	{
	private:
		ID3D10ShaderResourceView* view;

		unsigned int sizePixels;
		std::string name;

	public:
		Texture(const std::string& name, int sizePixels, ID3D10ShaderResourceView* view);

#ifdef _DEBUG
		virtual ~Texture();
#endif


		const std::string& GetName() const
		{
			return name;
		}

		ID3D10ShaderResourceView* GetShaderResourceView() 
		{ 
			return view; 
		}

		int GetSizePixels() const
		{
			return sizePixels;
		}
	};
}