#pragma once

#include <d3d10_1.h>
#include <string>

namespace Pimp
{
	class Texture
	{
	private:
		ID3D10ShaderResourceView* view;

		unsigned int width;
		unsigned int height;
		std::string name;

	public:
		Texture(const std::string& name, int width, int height, ID3D10ShaderResourceView* view);

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

		int GetWidth() const
		{
			return width;
		}

		int GetHeight() const
		{
			return height;
		}
	};
}