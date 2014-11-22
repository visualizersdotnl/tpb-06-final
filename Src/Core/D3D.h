#pragma once

#include <DXGI.h>
#include <d3d10_1.h>
#include <string>
#include <math/math.h>

#include "D3DAssert.h"
#include "Settings.h"
#include "RenderTarget.h"
#include "PIX.h"
#include "DepthStencil.h"
#include "Texture2D.h"
#include "Texture3D.h"

namespace Pimp
{
	class RenderTarget;
	class DepthStencil;

	class D3D
	{
	public:
		D3D(ID3D10Device1 *device, IDXGISwapChain* swapChain);
		~D3D();

		void Clear(ID3D10RenderTargetView* renderTarget);
		void ClearBackBuffer();
		void ClearDepthStencil();
		void Flip(UINT syncInterval);

		void SetBackViewport();    // Full back buffer viewport (swapchain resolution).
		void SetBackAdjViewport(); // Aspect ratio adjusted back buffer viewport (centered).
		void SetSceneViewport();   // Scene viewport (aspect ratio adjusted).

		// Resource creation
		ID3D10Buffer* CreateVertexBuffer(int numBytes, const void* initialData, bool isDynamic);
		ID3D10Buffer* CreateIndexBuffer(int numIndices, const void* initialData, bool isDynamic);

		ID3D10Effect* CreateEffect(const unsigned char* compiledEffect, int compiledEffectLength);

		bool CompileEffect(const unsigned char* effectAscii, int effectAsciiSize, unsigned char** outCompiledEffectBuffer, int* compiledEffectLength);

		ID3D10InputLayout* CreateInputLayout(	
			const D3D10_INPUT_ELEMENT_DESC* description,
			int numDescriptionItems,
			unsigned char* signatureIA, int signatureIALength);

		RenderTarget* CreateRenderTarget(int viewportShrinkFactor, DXGI_FORMAT format, bool multiSample);
		DepthStencil* CreateDepthStencil(bool multiSample);

		ID3D10Texture2D* CreateIntermediateCPUTarget(DXGI_FORMAT format);
		
		void CopyTextures(ID3D10Texture2D* dest, ID3D10Texture2D* src);

		Texture2D* CreateTexture2D(const std::string& name, int width, int height, bool requiresGammaCorrection);
		Texture3D* CreateTexture3D(const std::string& name, int width, int height, int depth);

		// Buffer binds
		void BindVertexBuffer(int slot, ID3D10Buffer* buffer, unsigned int stride);
		void BindIndexBuffer(ID3D10Buffer* buffer);
		void BindInputLayout(ID3D10InputLayout* layout);
		void BindBackbuffer(DepthStencil* depth);
		void BindRenderTarget(RenderTarget* pixels, DepthStencil* depth);
		void BindRenderTargetTexture3D(Texture3D* pixels, int sliceIndex);

		void ResolveMultiSampledRenderTarget(ID3D10Texture2D* pDest, ID3D10Texture2D* pSource, DXGI_FORMAT format);

		// Non-indexed draw methods
		void DrawTriList(DWORD numTris);
		void DrawTriQuad(DWORD offset);
		void DrawScreenQuad() { DrawTriQuad(0); } 

		// Indexed draw methods
		void DrawIndexedTriList(DWORD numTris);

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

		void SetBlendMode(Blend mode);

		// Small 4*4 full white texture
		Texture2D *GetWhiteTex() const { return pWhiteTex; }

	private:
		// These are supplied by on construction (not owned)
		ID3D10Device1* device;
		IDXGISwapChain* swapChain;

		// Actual resources
		ID3D10RasterizerState* rasterizerState;
		RenderTarget* renderTargetBackBuffer;
		DepthStencil* depthStencil;
		ID3D10DepthStencilState* depthStencilState[2]; // See D3D.cpp
		ID3D10BlendState* blendStates[MAX_BlendMode];
		Texture2D* pWhiteTex;

		// Viewports
		D3D10_VIEWPORT backVP;    // Full-size back buffer viewport.
		D3D10_VIEWPORT backAdjVP; // Viewport that centers scene on back buffer.
		D3D10_VIEWPORT sceneVP;   // Aspect-ratio adjusted full viewport.
	};

	extern D3D* gD3D;
}
