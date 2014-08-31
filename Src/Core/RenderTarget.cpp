#include "RenderTarget.h"


namespace Pimp 
{
	RenderTarget::RenderTarget(
		DXGI_FORMAT format,
		ID3D10Texture2D* texture, 
		ID3D10RenderTargetView* renderTargetView,
		ID3D10ShaderResourceView* shaderResourceView)
		: format(format), texture(texture), renderTargetView(renderTargetView),
		shaderResourceView(shaderResourceView)
	{
	}


	RenderTarget::~RenderTarget()
	{
		if (shaderResourceView != NULL)
			shaderResourceView->Release();

		renderTargetView->Release();
		texture->Release();
	}


	void RenderTarget::ResolveTo(RenderTarget* other)
	{
		gD3D->ResolveMultiSampledRenderTarget(other->texture, texture, format);
	}
}