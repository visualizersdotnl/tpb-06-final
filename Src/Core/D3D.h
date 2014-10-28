#pragma once

#include <DXGI.h>
#include <d3d10_1.h>
#include <string>
#include <math/math.h>

// @plek: As of now Core reports it's errors to it's only host: Player.
#include "../Player/SetLastError.h" 

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
		D3D(ID3D10Device1 *device, IDXGISwapChain* swapchain);
		~D3D();

		void Clear(ID3D10RenderTargetView* renderTarget, float R, float G, float B, float A);
		void ClearBackBuffer();
		void ClearDepthStencil();
		void Flip();

		ID3D10Buffer* CreateVertexBuffer(int numBytes, const void* initialData, bool isDynamic);
		ID3D10Buffer* CreateIndexBuffer(int numIndices, const void* initialData, bool isDynamic);

		void BindVertexBuffer(int slot, ID3D10Buffer* buffer, unsigned int stride);
		void BindIndexBuffer(ID3D10Buffer* buffer);
		void BindInputLayout(ID3D10InputLayout* layout);

		void BindBackbuffer(DepthStencil* depth);
		void BindRenderTarget(RenderTarget* pixels, DepthStencil* depth);
		void BindRenderTargetTexture3D(Texture3D* pixels, int sliceIndex);

		void DrawTriList(DWORD numTris);
		void DrawTriQuad(DWORD offset);
		void DrawScreenQuad() { DrawTriQuad(0); } 

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
		
		void UseDepthStencil(bool enabled);
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
			BM_Subtractive,
			MAX_BlendMode
		};

		void SetBlendMode(BlendMode blendMode);

		// This texture is quite useful for stuff like untextured sprites.
		// It's just 4*4 0xffffffff.
		Texture2D *GetWhiteTex() { return pWhiteTex; }

	private:
		int viewWidth, viewHeight;

		// These are supplied by host on construction, so don't release them!
		ID3D10Device1* device;
		IDXGISwapChain* swapchain;

		// Actual resources.
		ID3D10RasterizerState* rasterizerState;
		RenderTarget* renderTargetBackBuffer;
		DepthStencil* depthStencil;
		ID3D10DepthStencilState* depthStencilState[2]; // See D3D.cpp
		ID3D10BlendState* blendStates[MAX_BlendMode];
		Texture2D *pWhiteTex;

		// Scale (XY) to render with. Includes aspect ratio correction (letterboxing).
		Vector2 renderScale; 
		
		// Full and aspect ratio adjusted view port.
		D3D10_VIEWPORT m_fullVP, m_adjVP;
	};

	extern D3D* gD3D;
}
