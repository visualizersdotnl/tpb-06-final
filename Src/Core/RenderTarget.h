
#pragma once

namespace Pimp
{
	class RenderTarget
	{
	public:
		RenderTarget(
			DXGI_FORMAT format,
			ID3D11Texture2D* texture, 
			ID3D11RenderTargetView* renderTargetView,
			ID3D11ShaderResourceView* shaderResourceView);
		~RenderTarget();

		void ResolveTo(RenderTarget* other);

		ID3D11ShaderResourceView* GetShaderResourceView() const { return shaderResourceView; }
		ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView; }
		ID3D11Texture2D* GetTexture() const { return texture; }

	private:
		DXGI_FORMAT format;
		ID3D11Texture2D* texture;
		ID3D11RenderTargetView* renderTargetView;
		ID3D11ShaderResourceView* shaderResourceView;
	};
}
