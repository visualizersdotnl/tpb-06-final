
#include "Platform.h"
#include "D3D.h"
// #include "Texture2D.h"

namespace Pimp
{
	Texture2D::Texture2D(const std::string& name, int width, int height, ID3D11Texture2D* texture, ID3D11ShaderResourceView* view)
		: Texture(name, width, height, view), texture(texture)
	{
	}

	Texture2D::~Texture2D()
	{
		SAFE_RELEASE(texture);
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
