
#pragma once

namespace Pimp
{
	class Texture
	{
	private:
		ID3D11ShaderResourceView* view;

		const unsigned int width;
		const unsigned int height;
		const std::string name;

	public:
		Texture(const std::string& name, int width, int height, ID3D11ShaderResourceView* view);
		virtual ~Texture();

		const std::string& GetName() const
		{
			return name;
		}

		ID3D11ShaderResourceView* GetShaderResourceView() 
		{ 
			return view; 
		}

		int GetWidth() const  { return width;  }
		int GetHeight() const { return height; }
	};
}