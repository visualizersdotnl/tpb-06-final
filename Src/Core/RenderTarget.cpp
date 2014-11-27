
#include "Platform.h"
#include "D3D.h"
// #include "RenderTarget.h"

namespace Pimp 
{
	RenderTarget::RenderTarget(
		DXGI_FORMAT format,
		ID3D11Texture2D* texture, 
		ID3D11RenderTargetView* renderTargetView,
		ID3D11ShaderResourceView* shaderResourceView) :
		format(format), 
		texture(texture), 
		renderTargetView(renderTargetView),
		shaderResourceView(shaderResourceView)
	{
	}

	RenderTarget::~RenderTarget()
	{
		SAFE_RELEASE(shaderResourceView);
		SAFE_RELEASE(renderTargetView);
		SAFE_RELEASE(texture);
	}

	void RenderTarget::ResolveTo(RenderTarget* other)
	{
		ASSERT(nullptr != other);
		gD3D->ResolveMultiSampledRenderTarget(other->texture, texture, format);
	}
}
