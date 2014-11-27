
#pragma once

// FIXME: eventually remove this dependency for our own system as it's officially deprecated.
#include <Effects11/inc/d3dx11effect.h>

// #include "PIX.h"
#include "D3DAssert.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "Texture2D.h"
#include "Texture3D.h"

namespace Pimp
{
	class D3D
	{
	public:
		D3D(ID3D11Device *device, ID3D11DeviceContext *context, IDXGISwapChain* swapChain);
		~D3D();

		// FIXME: evaluate if still necessary after phasing out Effect11 library.
		ID3D11Device* GetDevice() { return device; }
		ID3D11DeviceContext* GetContext() { return context; }

		void Clear(ID3D11RenderTargetView* renderTarget);

		void ClearBackBuffer()
		{ 
			Clear(renderTargetBackBuffer->GetRenderTargetView()); 
		}

		void ClearDepthStencil()
		{
			context->ClearDepthStencilView(depthStencil->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
		}

		void Flip(UINT syncInterval)
		{
			D3D_VERIFY(swapChain->Present(syncInterval, 0));
		}

		// Check viewport definitions below for comments on their specifics.
		void SetBackViewport()    { context->RSSetViewports(1, &backVP);    }
		void SetBackAdjViewport() { context->RSSetViewports(1, &backAdjVP); }
		void SetSceneViewport()   { context->RSSetViewports(1, &sceneVP);   }

		// Buffer binds
		void BindVertexBuffer(int slot, ID3D11Buffer* buffer, unsigned int stride, unsigned int offset = 0)
		{
			context->IASetVertexBuffers(slot, 1, &buffer, &stride, &offset);
		}

		void BindIndexBuffer(ID3D11Buffer* buffer) // Fixed 32-bit. Get with the times.
		{
			context->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
		}

		void BindInputLayout(ID3D11InputLayout* layout)
		{
			context->IASetInputLayout(layout);
		}

		void BindBackbuffer(DepthStencil* depth);
		void BindRenderTarget(RenderTarget* pixels, DepthStencil* depth);
		void BindRenderTargetTexture3D(Texture3D* pixels, int sliceIndex);

		// Texture copy (2D)
		void CopyTextures(ID3D11Texture2D* pDest, ID3D11Texture2D* pSrc)
		{
			ASSERT(nullptr != pDest && nullptr != pSrc);
			context->CopyResource(pDest, pSrc);
		}

		// Multisample resolve
		void ResolveMultiSampledRenderTarget(ID3D11Texture2D* pDest, ID3D11Texture2D* pSrc, DXGI_FORMAT format)
		{
			ASSERT(nullptr != pDest && nullptr != pSrc);
			context->ResolveSubresource(pDest, 0, pSrc, 0, format);
		}

		// Non-indexed draw methods
		void DrawTriList(DWORD numTris) { context->Draw(numTris*3, 0); }
		void DrawTriQuad(DWORD offset)  { context->Draw(6, offset);    }
		void DrawScreenQuad()           { DrawTriQuad(0);             } 

		// Indexed draw method(s)
		void DrawIndexedTriList(DWORD numTris)
		{
			context->DrawIndexed(numTris*3, 0, 0);
		}
		
		// Full aspect ratio adjusted buffer size for rendering,
		// matches render target dimensions (not taking LOD into account).
		void GetRenderSize(float& width, float& height)
		{
			width = sceneVP.Width;
			height = sceneVP.Height;
		}

		void D3D::EnableDepthStencil()
		{
			context->OMSetDepthStencilState(depthStencilState[0], 0);
		}

		void D3D::DisableDepthStencil()
		{
			context->OMSetDepthStencilState(depthStencilState[1], 0);
		}

		DepthStencil* GetDefaultDepthStencil()    { return depthStencil; }
		RenderTarget* GetRenderTargetBackBuffer() { return renderTargetBackBuffer; }

		enum Blend
		{
			BM_None,
			BM_AlphaBlend,
			BM_Additive,
			BM_Subtractive,
			BM_AlphaMask,
			BM_AlphaPreMul,
			MAX_BlendMode
		};

		void SetBlendMode(Blend mode)
		{
			context->OMSetBlendState(blendStates[mode], nullptr, 0xffffffff);
		}

		// Small 4*4 full white texture
		Texture2D *GetWhiteTex() const { return pWhiteTex; }

		// Resource creation
		ID3D11Buffer* CreateVertexBuffer(int numBytes, const void* initialData, bool isDynamic);
		ID3D11Buffer* CreateIndexBuffer(int numIndices, const void* initialData, bool isDynamic);

		ID3D11InputLayout* CreateInputLayout(	
			const D3D11_INPUT_ELEMENT_DESC* description,
			int numDescriptionItems,
			unsigned char* signatureIA, int signatureIALength);

		ID3DX11Effect* CreateEffect(const unsigned char* compiledEffect, int compiledEffectLength);

		// Create aspect ratio adjusted size (sceneVP), shrunk by LOD factor
		RenderTarget* CreateRenderTarget(int shrinkFactor, DXGI_FORMAT format, bool multiSample);

		// Create aspect ratio adjusted size (sceneVP) CPU target
		ID3D11Texture2D* CreateIntermediateCPUTarget(DXGI_FORMAT format);
		
		DepthStencil* CreateDepthStencil(bool multiSample); // Full swap chain size.
		
		Texture2D* CreateTexture2D(const std::string& name, int width, int height, bool requiresGammaCorrection);
		Texture3D* CreateTexture3D(const std::string& name, int width, int height, int depth);

		// .FX compile (only for dev. usage, ship with precompiled shaders)
		bool CompileEffect(
			const std::string &path,
			const unsigned char* effectAscii, 
			int effectAsciiSize, 
			unsigned char** outCompiledEffectBuffer, 
			int* outCompiledEffectLength,
			std::string &errorMsg);

	private:
		// These are supplied by on construction (not owned)
		ID3D11Device* device;
		ID3D11DeviceContext* context;
		IDXGISwapChain* swapChain;

		// Actual resources
		ID3D11RasterizerState* rasterizerState;
		RenderTarget* renderTargetBackBuffer;
		DepthStencil* depthStencil;
		ID3D11DepthStencilState* depthStencilState[2]; // On (0) and off (1), plain and simple.
		ID3D11BlendState* blendStates[MAX_BlendMode];
		Texture2D* pWhiteTex;

		// Viewports
		D3D11_VIEWPORT backVP;    // Full-size back buffer viewport.
		D3D11_VIEWPORT backAdjVP; // Viewport that centers scene on back buffer.
		D3D11_VIEWPORT sceneVP;   // Aspect-ratio adjusted full viewport.

#if defined(_DEBUG)
		ID3D11InfoQueue* infoQueue;
#endif
	};

	extern D3D* gD3D;
}
