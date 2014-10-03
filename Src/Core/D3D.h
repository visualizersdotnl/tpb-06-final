#pragma once

#include <DXGI.h>
#include <d3d10_1.h>
// #include <string>
#include <math/math.h>

// @plek: I'm making short work of C++ exceptions.
#include "../KillUnwindSemantics.h"

// @plek: As of now Core reports it's errors to it's only host: Player.
//        Maybe SetLastError() has a place in Shared but for now this will do.
#include "../Player/SetLastError.h" 

#include "APIAssert.h"
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
		D3D(ID3D10Device1 *device, IDXGISwapChain* swapchain);
		~D3D();

		void Clear(ID3D10RenderTargetView* renderTarget);
		void ClearBackBuffer();
		void ClearDepthStencil();
		void Flip();

		ID3D10Buffer* CreateVertexBuffer(int numBytes, const void* initialData);
		ID3D10Buffer* CreateIndicesBuffer(int numIndices, const void* initialData);

		void BindVertexBuffer(int slot, ID3D10Buffer* buffer, unsigned int stride);
		void BindIndexBuffer(ID3D10Buffer* buffer);
		void BindInputLayout(ID3D10InputLayout* layout);

		void BindBackbuffer(DepthStencil* depth);
		void BindRenderTarget(RenderTarget* pixels, DepthStencil* depth);
		void BindRenderTargetTexture3D(Texture3D* pixels, int sliceIndex);

		void DrawScreenQuad();

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

		void ResolveMultiSampledRenderTarget(
			ID3D10Texture2D* dest,
			ID3D10Texture2D* source,
			DXGI_FORMAT format);

		void GetViewportSize(int* width, int* height);

		DepthStencil* GetDefaultDepthStencil() const;

		const Vector2& GetRenderScale() const 
		{ 
			return renderScale;
		}

		RenderTarget* GetRenderTargetBackBuffer() const
		{
			return renderTargetBackBuffer;
		}

		enum BlendMode
		{
			BM_None,
			BM_AlphaBlend,
			BM_Additive,
			MAX_BlendMode
		};

		void SetBlendMode(BlendMode blendMode);

	private:
		int viewWidth, viewHeight;

		// These are supplied by host on construction, so don't release them!
		ID3D10Device1* device;
		IDXGISwapChain* swapchain;

		// Actual resources.
		ID3D10RasterizerState* rasterizerState;
		RenderTarget* renderTargetBackBuffer;
		DepthStencil* depthStencil;
		ID3D10DepthStencilState* depthStencilState;
		ID3D10BlendState* blendStates[MAX_BlendMode];

		// Scale (XY) to render with. Includes aspect ratio correction (letterboxing).
		Vector2 renderScale; 
	};

	extern D3D* gD3D;
}
