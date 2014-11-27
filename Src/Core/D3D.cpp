
// FIXME: location is deprecated.
// Header included in D3DAssert.h
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x64\\DxErr.lib")


#include "Platform.h"
#include <D3DCompiler.h>
#include "D3D.h"
#include "Settings.h"
#include "Configuration.h"

namespace Pimp
{

// Convenience hack.
D3D* gD3D = nullptr;


D3D::D3D(ID3D11Device *device, ID3D11DeviceContext *context, IDXGISwapChain* swapChain) : 
	device(device), context(context), swapChain(swapChain)
,	rasterizerState(nullptr)
,	renderTargetBackBuffer(nullptr)
,	sceneDepthStencil(nullptr)
,	pWhiteTex(nullptr)
{
	depthStencilState[0] = depthStencilState[1] = nullptr;
	memset(blendStates, 0, MAX_BlendMode*sizeof(Blend));

	HRESULT hRes = S_OK;

	const Configuration::DisplayMode& mode = Configuration::Instance()->GetDisplayMode();
	const float viewWidth = (float) mode.width;
	const float viewHeight = (float) mode.height;

#if defined(_DEBUG) && D3D_DISABLE_SPECIFIC_WARNINGS
	infoQueue = nullptr;
	ASSERT(S_OK == device->QueryInterface(__uuidof(ID3D11InfoQueue), (void **)&infoQueue));
	
	D3D11_MESSAGE_ID messageIDs [] = { 
		D3D11_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD,
		D3D11_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD,
	};

	// Set the DenyList to use the list of messages
	D3D11_INFO_QUEUE_FILTER filter = { 0 };
	filter.DenyList.NumIDs = 2;
	filter.DenyList.pIDList = messageIDs;

	// Apply the filter to the info queue
	infoQueue->AddStorageFilterEntries(&filter);  
#endif

	// Retrieve backbuffer
	ID3D11Texture2D* backbuffer;
	hRes = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);
	D3D_ASSERT(hRes);

	// Create render target view for backbuffer
	ID3D11RenderTargetView* renderTargetViewBackbuffer;
	hRes = device->CreateRenderTargetView(backbuffer, NULL, &renderTargetViewBackbuffer);
	D3D_ASSERT(hRes);	ASSERT(renderTargetViewBackbuffer != NULL);

	renderTargetBackBuffer = new RenderTarget(PIMP_BACKBUFFER_FORMAT_GAMMA, backbuffer, renderTargetViewBackbuffer, NULL);

	// Resort to triangle lists
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create and set fixed rasterizer state
	D3D11_RASTERIZER_DESC rasterDesc;
	memset(&rasterDesc, 0, sizeof(rasterDesc));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0;
	rasterDesc.SlopeScaledDepthBias = 0;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.ScissorEnable = FALSE;
	rasterDesc.MultisampleEnable = TRUE;
	rasterDesc.AntialiasedLineEnable = FALSE;
	D3D_VERIFY(device->CreateRasterizerState(&rasterDesc, &rasterizerState));

	context->RSSetState(rasterizerState);

	// Define full back buffer viewport
	backVP.TopLeftX = 0.f;
	backVP.TopLeftY = 0.f;
	backVP.Width = viewWidth;
	backVP.Height = viewHeight;
	backVP.MinDepth = 0.f;
	backVP.MaxDepth = 1.f;

	// Calculate viewports (full & aspect ratio adjusted)
	const float fullAspectRatio = backVP.Width / backVP.Height;
	const float renderAspectRatio = Configuration::Instance()->GetRenderAspectRatio();

	float xResAdj, yResAdj;
	if (fullAspectRatio < renderAspectRatio)
	{
		const float scale = fullAspectRatio / renderAspectRatio;
		xResAdj = backVP.Width;
		yResAdj = backVP.Height*scale;
	}
	else if (fullAspectRatio > renderAspectRatio)
	{
		const float scale = renderAspectRatio / fullAspectRatio;
		xResAdj = backVP.Width*scale;
		yResAdj = backVP.Height;
	}
	else // ==
	{
		xResAdj = backVP.Width;
		yResAdj = backVP.Height;
	}

	// Backbuffer viewport adjusted to fit (centered) scene
	backAdjVP.Width = xResAdj;
	backAdjVP.Height = yResAdj;
	backAdjVP.TopLeftX = (backVP.Width-xResAdj)/2.f;
	backAdjVP.TopLeftY = (backVP.Height-yResAdj)/2.f;
	backAdjVP.MinDepth = 0.f;
	backAdjVP.MaxDepth = 1.f;

	// Scene viewport
	sceneVP.Width = xResAdj;
	sceneVP.Height = yResAdj;
	sceneVP.TopLeftX = 0.f;
	sceneVP.TopLeftY = 0.f;
	sceneVP.MinDepth = 0.f;
	sceneVP.MaxDepth = 1.f;

	// Set full viewport by default
	context->RSSetViewports(1, &backVP);

	// Create default scene depth-stencil
	sceneDepthStencil = CreateSceneDepthStencil(true);

	// Depth-stencil states (on and off)
	D3D11_DEPTH_STENCIL_DESC dsStateDesc;
	memset(&dsStateDesc, 0, sizeof(dsStateDesc));

	dsStateDesc.DepthEnable = true;
	dsStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
	D3D_VERIFY(device->CreateDepthStencilState(&dsStateDesc, &depthStencilState[0]));
	
	dsStateDesc.DepthEnable = false;
	dsStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsStateDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	D3D_VERIFY(device->CreateDepthStencilState(&dsStateDesc, &depthStencilState[1]));

	// Depth-stencil disabled by default
	DisableSceneDepthStencil();

	// Bind backbuffer as primary render target
	BindBackbuffer();

	// Create blend states
	//

	blendStates[BM_None] = nullptr;

	D3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = FALSE; // FIXME: read up on this!
	blendDesc.IndependentBlendEnable = FALSE;

	D3D11_RENDER_TARGET_BLEND_DESC &alphaDesc = blendDesc.RenderTarget[0];
	alphaDesc.BlendEnable = TRUE;
	alphaDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	alphaDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	alphaDesc.BlendOp = D3D11_BLEND_OP_ADD;
	alphaDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	alphaDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	alphaDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	alphaDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blendDesc, &blendStates[BM_AlphaBlend]);

	D3D11_RENDER_TARGET_BLEND_DESC &addDesc = blendDesc.RenderTarget[0];
	addDesc.BlendEnable = TRUE;
	addDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	addDesc.DestBlend = D3D11_BLEND_ONE;	
	addDesc.BlendOp = D3D11_BLEND_OP_ADD;
	addDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	addDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	addDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	addDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blendDesc, &blendStates[BM_Additive]);

	D3D11_RENDER_TARGET_BLEND_DESC &subDesc = blendDesc.RenderTarget[0];
	subDesc.BlendEnable = TRUE;
	subDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	subDesc.DestBlend = D3D11_BLEND_ONE;
	subDesc.BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	subDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	subDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	subDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	subDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blendDesc, &blendStates[BM_Subtractive]);

	D3D11_RENDER_TARGET_BLEND_DESC &alphaMaskDesc = blendDesc.RenderTarget[0];
	alphaMaskDesc.BlendEnable = TRUE;
	alphaMaskDesc.SrcBlend = D3D11_BLEND_ZERO;
	alphaMaskDesc.DestBlend = D3D11_BLEND_SRC_ALPHA;
	alphaMaskDesc.BlendOp = D3D11_BLEND_OP_ADD;
	alphaMaskDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	alphaMaskDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	alphaMaskDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	alphaMaskDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    device->CreateBlendState(&blendDesc, &blendStates[BM_AlphaMask]);

	D3D11_RENDER_TARGET_BLEND_DESC &alphaPreMulDesc = blendDesc.RenderTarget[0];
	alphaPreMulDesc.BlendEnable = TRUE;
	alphaPreMulDesc.SrcBlend = D3D11_BLEND_ONE;
	alphaPreMulDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	alphaPreMulDesc.BlendOp = D3D11_BLEND_OP_ADD;
	alphaPreMulDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	alphaPreMulDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	alphaPreMulDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	alphaPreMulDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    device->CreateBlendState(&blendDesc, &blendStates[BM_AlphaPreMul]);

	// Create default (white) texture
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = 4;
		texDesc.Height = 4;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		unsigned int whitePixels[4*4];
		memset(whitePixels, -1, 4*4*sizeof(int));

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = whitePixels;
		data.SysMemPitch = 4*sizeof(int);
		data.SysMemSlicePitch = 0;

		ID3D11Texture2D *pResource;
		D3D_VERIFY(device->CreateTexture2D(&texDesc, &data, &pResource));

		ID3D11ShaderResourceView *pView;
		D3D_VERIFY(device->CreateShaderResourceView(pResource, NULL, &pView));

		pWhiteTex = new Texture2D("texWhite", 4, 4, pResource, pView);
	}
}

D3D::~D3D()
{
#if defined(_DEBUG) && D3D_DISABLE_SPECIFIC_WARNINGS
	SAFE_RELEASE(infoQueue);
#endif

	delete pWhiteTex;

	for (size_t iBS = 0; iBS < MAX_BlendMode; ++iBS)
		SAFE_RELEASE(blendStates[iBS]);
	
	SAFE_RELEASE(depthStencilState[0]);
	SAFE_RELEASE(depthStencilState[1]);

	delete sceneDepthStencil;
	SAFE_RELEASE(rasterizerState);
	delete renderTargetBackBuffer;
}

void D3D::Clear(ID3D11RenderTargetView* renderTarget)
{
	const float black[4] = { 0.f };
	context->ClearRenderTargetView(renderTarget, black);
}

void D3D::BindBackbuffer()
{
	ID3D11RenderTargetView* view = renderTargetBackBuffer->GetRenderTargetView();
	context->OMSetRenderTargets(1, &view, nullptr);
}

void D3D::BindRenderTarget(RenderTarget* pixels, DepthStencil* depth)
{
	ASSERT(nullptr != pixels);
	ID3D11RenderTargetView* view = pixels->GetRenderTargetView();
	context->OMSetRenderTargets(1, &view, (nullptr != depth) ? depth->GetDepthStencilView() : nullptr);
}

void D3D::BindRenderTargetTexture3D(Texture3D* pixels, int sliceIndex)
{
	ASSERT(nullptr != pixels && sliceIndex >= 0);
	ID3D11RenderTargetView* view = pixels->GetRenderTargetView(sliceIndex);
	context->OMSetRenderTargets(1, &view, nullptr); // No depth-stencil.
}

ID3D11Buffer* D3D::CreateVertexBuffer(int numBytes, const void* initialData, bool isDynamic)
{
	ASSERT(numBytes > 0);

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = (isDynamic) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = numBytes;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = (isDynamic) ? D3D11_CPU_ACCESS_WRITE : 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = initialData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	ID3D11Buffer* buffer = nullptr;
	const HRESULT hRes = device->CreateBuffer(&bufferDesc, (initialData == nullptr) ? nullptr : &data, &buffer);
	D3D_ASSERT(hRes);
	
	return buffer;
}

ID3D11Buffer* D3D::CreateIndexBuffer(int numIndices, const void* initialData, bool isDynamic)
{
	ASSERT(numIndices > 0);

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = (isDynamic) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = numIndices * sizeof(DWORD);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = (isDynamic) ? D3D11_CPU_ACCESS_WRITE : 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	if (false == isDynamic)
	{
		data.pSysMem = initialData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
	}

	ID3D11Buffer* buffer = nullptr;
	const HRESULT hRes = device->CreateBuffer(&bufferDesc, (true == isDynamic) ? nullptr : &data, &buffer);
	D3D_ASSERT(hRes);

	return buffer;
}

ID3D11InputLayout* D3D::CreateInputLayout(
	const D3D11_INPUT_ELEMENT_DESC* description,
	int numDescriptionItems,
	unsigned char* signatureIA, int signatureIALength)
{
	ASSERT(nullptr != description);

	ID3D11InputLayout* layout = nullptr;
	const HRESULT hRes = device->CreateInputLayout(
		description, 
		numDescriptionItems, 
		signatureIA, 
		signatureIALength,
		&layout);
	D3D_ASSERT(hRes);

	return layout;
}

ID3DX11Effect* D3D::CreateEffect(const unsigned char* compiledEffect, int compiledEffectLength)
{
	ASSERT(nullptr != compiledEffect && compiledEffectLength > 0);

	ID3DX11Effect* effect = nullptr;
	const HRESULT hRes = D3DX11CreateEffectFromMemory(
		static_cast<LPCVOID>(compiledEffect), compiledEffectLength,
		0, // Flags
		device, &effect);
	D3D_ASSERT(hRes);

	return effect;
}

RenderTarget* D3D::CreateRenderTarget(int shrinkFactor, DXGI_FORMAT format, bool multiSample)
{
	ASSERT(shrinkFactor >= 1);

	// Create texture
	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(texDesc));
	texDesc.Width  = (UINT) sceneVP.Width  / shrinkFactor;
	texDesc.Height = (UINT) sceneVP.Height / shrinkFactor;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = multiSample ? D3D_ANTIALIAS_NUM_SAMPLES : 1;
	texDesc.SampleDesc.Quality = multiSample ? D3D_ANTIALIAS_QUALITY : 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	ID3D11Texture2D* texture = nullptr;
	HRESULT hRes = device->CreateTexture2D(&texDesc, NULL, &texture);
	D3D_ASSERT(hRes);

	// Create view
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	memset(&rtvDesc, 0, sizeof(rtvDesc));
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS; // Multi-sampled!

	ID3D11RenderTargetView* renderTargetView = nullptr;
	hRes = device->CreateRenderTargetView(texture, nullptr, &renderTargetView);
	D3D_ASSERT(hRes);

	// FIXME: if I do this my loading bar disappears!
//	ID3D11RenderTargetView* renderTargetView = nullptr;
//	hRes = device->CreateRenderTargetView(texture, (false == multiSample) ? nullptr : &rtvDesc, &renderTargetView);
//	D3D_ASSERT(hRes);

	// Create shader view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS; // Multi-sampled!

	ID3D11ShaderResourceView* shaderResourceView = nullptr;
	hRes = device->CreateShaderResourceView(texture, nullptr, &shaderResourceView);
	D3D_ASSERT(hRes);

	// FIXME: if I do this my loading bar disappears!
//	ID3D11ShaderResourceView* shaderResourceView = nullptr;
//	hRes = device->CreateShaderResourceView(texture, (false == multiSample) ? nullptr : &srvDesc, &shaderResourceView);
//	D3D_ASSERT(hRes);

	return new RenderTarget(format, texture, renderTargetView, shaderResourceView);

	// FIXME: if it cops out anywhere here by way of throw we're obviously potentially leaking.
}

ID3D11Texture2D* D3D::CreateIntermediateCPUTarget(DXGI_FORMAT format)
{
	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(texDesc));
	texDesc.Width = (UINT) sceneVP.Width;
	texDesc.Height = (UINT) sceneVP.Height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	texDesc.BindFlags = 0;

	ID3D11Texture2D* texture = nullptr;
	const HRESULT hRes = device->CreateTexture2D(&texDesc, nullptr, &texture);
	D3D_ASSERT(hRes);

	return texture;
}

DepthStencil* D3D::CreateSceneDepthStencil(bool multiSample)
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = (UINT) sceneVP.Width;
	texDesc.Height = (UINT) sceneVP.Height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texDesc.SampleDesc.Count = multiSample ? D3D_ANTIALIAS_NUM_SAMPLES : 1;
	texDesc.SampleDesc.Quality = multiSample ? D3D_ANTIALIAS_QUALITY : 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// Create depth stencil
	ID3D11Texture2D* texture = nullptr;
	HRESULT hRes = device->CreateTexture2D(&texDesc, nullptr, &texture);
	D3D_ASSERT(hRes);

	// And it's view
	ID3D11DepthStencilView* view = nullptr;

	D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipSlice = 0;
	
	hRes = device->CreateDepthStencilView(texture, nullptr /* FIXME: why not &viewDesc? */, &view);
	D3D_ASSERT(hRes);

	return new DepthStencil(texture, view);

	// FIXME: if it cops out anywhere here by way of throw we're obviously potentially leaking.
}

Texture2D* D3D::CreateTexture2D(const std::string& name, int width, int height, bool requiresGammaCorrection)
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = texDesc.ArraySize = 1; // Only a single mip
	texDesc.Format = requiresGammaCorrection ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DYNAMIC;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* texture = nullptr;
	HRESULT hRes = device->CreateTexture2D(&texDesc, nullptr, &texture);
	D3D_ASSERT(hRes);

	ID3D11ShaderResourceView* view = nullptr;
	hRes = device->CreateShaderResourceView(texture, nullptr, &view);
	D3D_ASSERT(hRes);

	return new Texture2D(name, width, height, texture, view);

	// FIXME: if it cops out anywhere here by way of throw we're obviously potentially leaking.
}

Texture3D* D3D::CreateTexture3D(const std::string& name, int width, int height, int depth)
{
	D3D11_TEXTURE3D_DESC texDesc;
	memset(&texDesc, 0, sizeof(texDesc));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.Depth = depth;
	texDesc.MipLevels = 1; // Only a single mip
	texDesc.Format = DXGI_FORMAT_R16G16_FLOAT;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; 
	// ^^ All our 3D textures are render targets too!

	ID3D11Texture3D* texture = nullptr;
	HRESULT hRes = device->CreateTexture3D(&texDesc, nullptr, &texture);
	D3D_ASSERT(hRes);

	// Create texture view
	ID3D11ShaderResourceView* view = nullptr;
	hRes = device->CreateShaderResourceView(texture, nullptr, &view);
	D3D_ASSERT(hRes);

	// Create N render target views
	ID3D11RenderTargetView** rtViews = new ID3D11RenderTargetView*[depth];

	for (int iSlice = 0; iSlice < depth; ++iSlice)
	{
		D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
		viewDesc.Format = texDesc.Format;
		viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
		viewDesc.Texture3D.MipSlice = 0;
		viewDesc.Texture3D.WSize = 1;
		viewDesc.Texture3D.FirstWSlice = iSlice;

		ID3D11RenderTargetView* renderTargetView = nullptr;
		hRes = device->CreateRenderTargetView(texture, &viewDesc, &renderTargetView);
		D3D_ASSERT(hRes);

		rtViews[iSlice] = renderTargetView;
	}

	return new Texture3D(name, width, height, depth, texture, view, rtViews);

	// FIXME: if it cops out anywhere here by way of throw we're obviously potentially leaking.
}

// FIXME: probably defined after porting to D3D11.
#ifndef D3D_COMPILE_STANDARD_FILE_INCLUDE
	#define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1)
#endif

bool D3D::CompileEffect(
	const std::string &path,
	const unsigned char* effectAscii, 
	int effectAsciiSize, 
	unsigned char** outCompiledEffectBuffer, 
	int* outCompiledEffectLength,
	std::string &errorMsg)
{	
	ID3DBlob* shader = nullptr;
	ID3DBlob* errors = nullptr;

	const HRESULT hRes = D3DCompile(
		effectAscii, 
		effectAsciiSize,
		path.c_str(),                     
		nullptr,                           // No defines.
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // Use default include handler.
		nullptr,                           // Default entry point.
		"fx_5_0",                          // Only target supported by Effects11.
		D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_SKIP_VALIDATION, 
		0,
		&shader,
		&errors);

	if (S_OK != hRes)
	{
		if (nullptr != errors)
		{			
			errorMsg = std::string(reinterpret_cast<char*>(errors->GetBufferPointer()), (size_t) errors->GetBufferSize());

			// Not using D3D_ASSERT_MSG as this may be called from a worker thread.
			ASSERT_MSG(S_OK != hRes, errorMsg.c_str());
		}

		SAFE_RELEASE(errors);
		return false;
	}

	ASSERT(shader->GetBufferSize() > 0);
	*outCompiledEffectBuffer = new unsigned char[shader->GetBufferSize()];
	ASSERT(*outCompiledEffectBuffer != NULL);
	memcpy(*outCompiledEffectBuffer, (unsigned char*)shader->GetBufferPointer(), (size_t) shader->GetBufferSize());
	*outCompiledEffectLength = (int)shader->GetBufferSize();

	shader->Release();
	return true;
}


} // namespace Pimp
