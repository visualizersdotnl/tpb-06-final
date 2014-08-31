#pragma once

#include "D3D.h"

namespace Pimp
{
	class RenderTarget
	{
	public:
		RenderTarget(
			DXGI_FORMAT format,
			ID3D10Texture2D* texture, 
			ID3D10RenderTargetView* renderTargetView,
			ID3D10ShaderResourceView* shaderResourceView);
		~RenderTarget();

		void ResolveTo(RenderTarget* other);

		ID3D10ShaderResourceView* GetShaderResourceView() const { return shaderResourceView; }
		ID3D10RenderTargetView* GetRenderTargetView() const { return renderTargetView; }
		ID3D10Texture2D* GetTexture() const { return texture; }

	private:
		DXGI_FORMAT format;
		ID3D10Texture2D* texture;
		ID3D10RenderTargetView* renderTargetView;
		ID3D10ShaderResourceView* shaderResourceView;
	};
}

