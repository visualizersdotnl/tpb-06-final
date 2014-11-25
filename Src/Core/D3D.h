
#pragma once

#include "PIX.h"

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
		D3D(ID3D10Device1 *device, IDXGISwapChain* swapChain);
		~D3D();

		void Clear(ID3D10RenderTargetView* renderTarget);

		void ClearBackBuffer()
		{ 
			Clear(renderTargetBackBuffer->GetRenderTargetView()); 
		}

		void ClearDepthStencil()
		{
			device->ClearDepthStencilView(depthStencil->GetDepthStencilView(), D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.f, 0);
		}

		void Flip(UINT syncInterval)
		{
			D3D_VERIFY(swapChain->Present(syncInterval, 0));
		}

		// Check viewport definitions below for comments on their specifics.
		void SetBackViewport()    { device->RSSetViewports(1, &backVP);    }
		void SetBackAdjViewport() { device->RSSetViewports(1, &backAdjVP); }
		void SetSceneViewport()   { device->RSSetViewports(1, &sceneVP);   }

		// Buffer binds
		void BindVertexBuffer(int slot, ID3D10Buffer* buffer, unsigned int stride, unsigned int offset = 0)
		{
			device->IASetVertexBuffers(slot, 1, &buffer, &stride, &offset);
		}

		void BindIndexBuffer(ID3D10Buffer* buffer) // Fixed 32-bit. Get with the times.
		{
			device->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
		}

		void BindInputLayout(ID3D10InputLayout* layout)
		{
			device->IASetInputLayout(layout);
		}

		void BindBackbuffer(DepthStencil* depth);
		void BindRenderTarget(RenderTarget* pixels, DepthStencil* depth);
		void BindRenderTargetTexture3D(Texture3D* pixels, int sliceIndex);

		// Texture copy (2D)
		void CopyTextures(ID3D10Texture2D* pDest, ID3D10Texture2D* pSrc)
		{
			ASSERT(nullptr != pDest && nullptr != pSrc);
			device->CopyResource(pDest, pSrc);
		}

		// Multisample resolve
		void ResolveMultiSampledRenderTarget(ID3D10Texture2D* pDest, ID3D10Texture2D* pSrc, DXGI_FORMAT format)
		{
			ASSERT(nullptr != pDest && nullptr != pSrc);
			device->ResolveSubresource(pDest, 0, pSrc, 0, format);
		}

		// Non-indexed draw methods
		void DrawTriList(DWORD numTris) { device->Draw(numTris*3, 0); }
		void DrawTriQuad(DWORD offset)  { device->Draw(6, offset);    }
		void DrawScreenQuad()           { DrawTriQuad(0);             } 

		// Indexed draw method(s)
		void DrawIndexedTriList(DWORD numTris)
		{
			device->DrawIndexed(numTris*3, 0, 0);
		}
		
		// Full aspect ratio adjusted buffer size for rendering,
		// matches render target dimensions (not taking LOD into account).
		void GetRenderSize(DWORD& width, DWORD& height)
		{
			width = sceneVP.Width;
			height = sceneVP.Height;
		}

		void D3D::EnableDepthStencil()
		{
			device->OMSetDepthStencilState(depthStencilState[0], 0);
		}

		void D3D::DisableDepthStencil()
		{
			device->OMSetDepthStencilState(depthStencilState[1], 0);
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
			device->OMSetBlendState(blendStates[mode], nullptr, 0xffffffff);
		}

		// Small 4*4 full white texture
		Texture2D *GetWhiteTex() const { return pWhiteTex; }

		// Resource creation
		ID3D10Buffer* CreateVertexBuffer(int numBytes, const void* initialData, bool isDynamic);
		ID3D10Buffer* CreateIndexBuffer(int numIndices, const void* initialData, bool isDynamic);

		ID3D10InputLayout* CreateInputLayout(	
			const D3D10_INPUT_ELEMENT_DESC* description,
			int numDescriptionItems,
			unsigned char* signatureIA, int signatureIALength);

		ID3D10Effect* CreateEffect(const unsigned char* compiledEffect, int compiledEffectLength);

		// Create aspect ratio adjusted size (sceneVP), shrunk by LOD factor
		RenderTarget* CreateRenderTarget(int shrinkFactor, DXGI_FORMAT format, bool multiSample);

		// Create aspect ratio adjusted size (sceneVP) CPU target
		ID3D10Texture2D* CreateIntermediateCPUTarget(DXGI_FORMAT format);
		
		DepthStencil* CreateDepthStencil(bool multiSample); // Full swap chain size.
		
		Texture2D* CreateTexture2D(const std::string& name, int width, int height, bool requiresGammaCorrection);
		Texture3D* CreateTexture3D(const std::string& name, int width, int height, int depth);

		// .FX compile (only for dev. usage, ship with precompiled shaders)
		bool CompileEffect(
			const unsigned char* effectAscii, 
			int effectAsciiSize, 
			unsigned char** outCompiledEffectBuffer, 
			int* outCompiledEffectLength,
			std::string &errorMsg);

	private:
		// These are supplied by on construction (not owned)
		ID3D10Device1* device;
		IDXGISwapChain* swapChain;

		// Actual resources
		ID3D10RasterizerState* rasterizerState;
		RenderTarget* renderTargetBackBuffer;
		DepthStencil* depthStencil;
		ID3D10DepthStencilState* depthStencilState[2]; // On (0) and off (1), plain and simple.
		ID3D10BlendState* blendStates[MAX_BlendMode];
		Texture2D* pWhiteTex;

		// Viewports
		D3D10_VIEWPORT backVP;    // Full-size back buffer viewport.
		D3D10_VIEWPORT backAdjVP; // Viewport that centers scene on back buffer.
		D3D10_VIEWPORT sceneVP;   // Aspect-ratio adjusted full viewport.

#if defined(_DEBUG) && D3D_DISABLE_SPECIFIC_WARNINGS
		ID3D10InfoQueue* infoQueue;
#endif
	};

	extern D3D* gD3D;
}
