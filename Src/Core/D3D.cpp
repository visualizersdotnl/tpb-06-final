
// FIXME: this is deprecated.
// Header included in D3DAssert.h
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x64\\DxErr.lib")


#include "Platform.h"
//#include <shared/shared.h>
#include "D3D.h"
#include "Settings.h"
#include "Configuration.h"
#include "D3DCompiler.h"

namespace Pimp
{

// Convenience hack.
D3D* gD3D = nullptr;


D3D::D3D(ID3D10Device1 *device, IDXGISwapChain* swapChain) : 
	device(device), swapChain(swapChain)
,	rasterizerState(nullptr)
,	renderTargetBackBuffer(nullptr)
,	depthStencil(nullptr)
,	pWhiteTex(nullptr)
{
	depthStencilState[0] = depthStencilState[1] = nullptr;
	memset(blendStates, 0, MAX_BlendMode*sizeof(Blend));

	HRESULT hRes = S_OK;

	const Configuration::DisplayMode& mode = Configuration::Instance()->GetDisplayMode();
	const DWORD viewWidth = mode.width;
	const DWORD viewHeight = mode.height;

#ifdef D3D_DISABLE_SPECIFIC_WARNINGS
	ID3D10InfoQueue* infoQueue;
	device->QueryInterface(__uuidof(ID3D10InfoQueue),  (void **)&infoQueue); 
	
	D3D10_MESSAGE_ID messageIDs [] = { 
		D3D10_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD,
		D3D10_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD,
	};

	// Set the DenyList to use the list of messages
	D3D10_INFO_QUEUE_FILTER filter = { 0 };
	filter.DenyList.NumIDs = 2;
	filter.DenyList.pIDList = messageIDs;

	// Apply the filter to the info queue
	infoQueue->AddStorageFilterEntries(&filter);  
#endif

	// Retrieve backbuffer
	ID3D10Texture2D* backbuffer;
	hRes = swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backbuffer);
	D3D_ASSERT(hRes);

	// Create render target view for backbuffer
	ID3D10RenderTargetView* renderTargetViewBackbuffer;
	hRes = device->CreateRenderTargetView(backbuffer, NULL, &renderTargetViewBackbuffer);
	D3D_ASSERT(hRes);	ASSERT(renderTargetViewBackbuffer != NULL);

	renderTargetBackBuffer = new RenderTarget(PIMP_BACKBUFFER_FORMAT_GAMMA, backbuffer, renderTargetViewBackbuffer, NULL);

	// Resort to triangle lists
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create and set fixed rasterizer state
	D3D10_RASTERIZER_DESC rasterDesc;
	memset(&rasterDesc, 0, sizeof(rasterDesc));
	rasterDesc.FillMode = D3D10_FILL_SOLID;
	rasterDesc.CullMode = D3D10_CULL_BACK;
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0;
	rasterDesc.SlopeScaledDepthBias = 0;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.ScissorEnable = FALSE;
	rasterDesc.MultisampleEnable = TRUE;
	rasterDesc.AntialiasedLineEnable = FALSE;
	D3D_VERIFY(device->CreateRasterizerState(&rasterDesc, &rasterizerState));

	device->RSSetState(rasterizerState);

	// Define full back buffer viewport
	backVP.TopLeftX = 0;
	backVP.TopLeftY = 0;
	backVP.Width = viewWidth;
	backVP.Height = viewHeight;
	backVP.MinDepth = 0.f;
	backVP.MaxDepth = 1.f;

	// Calculate viewports (full & aspect ratio adjusted)
	const float fullAspectRatio = (float) backVP.Width / backVP.Height;
	const float renderAspectRatio = Configuration::Instance()->GetRenderAspectRatio();

	unsigned int xResAdj, yResAdj;
	if (fullAspectRatio < renderAspectRatio)
	{
		const float scale = fullAspectRatio / renderAspectRatio;
		xResAdj = backVP.Width;
		yResAdj = (unsigned int) (backVP.Height*scale);
	}
	else if (fullAspectRatio > renderAspectRatio)
	{
		const float scale = renderAspectRatio / fullAspectRatio;
		xResAdj = (unsigned int) (backVP.Width*scale);
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
	backAdjVP.TopLeftX = (backVP.Width-xResAdj)/2;
	backAdjVP.TopLeftY = (backVP.Height-yResAdj)/2;
	backAdjVP.MinDepth = 0.f;
	backAdjVP.MaxDepth = 1.f;

	// Scene viewport
	sceneVP.Width = xResAdj;
	sceneVP.Height = yResAdj;
	sceneVP.TopLeftX = 0;
	sceneVP.TopLeftY = 0;
	sceneVP.MinDepth = 0.f;
	sceneVP.MaxDepth = 1.f;

	// Set full viewport by default
	device->RSSetViewports(1, &backVP);

	// Create depth-stencil
	depthStencil = CreateDepthStencil(true);

	// Depth-stencil states (on and off)
	D3D10_DEPTH_STENCIL_DESC dsStateDesc;
	memset(&dsStateDesc, 0, sizeof(dsStateDesc));

	dsStateDesc.DepthEnable = true;
	dsStateDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	dsStateDesc.DepthFunc = D3D10_COMPARISON_LESS;
	D3D_VERIFY(device->CreateDepthStencilState(&dsStateDesc, &depthStencilState[0]));
	
	dsStateDesc.DepthEnable = false;
	dsStateDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ZERO;
	dsStateDesc.DepthFunc = D3D10_COMPARISON_ALWAYS;
	D3D_VERIFY(device->CreateDepthStencilState(&dsStateDesc, &depthStencilState[1]));

	// Depth-stencil disabled by default
	DisableDepthStencil();

	// Bind backbuffer and depth stencil as primary RT
	BindBackbuffer(depthStencil);

	// Create blend states
	//

	blendStates[BM_None] = NULL;

	D3D10_BLEND_DESC alphaDesc;
	alphaDesc.AlphaToCoverageEnable = FALSE;
	alphaDesc.BlendEnable[0] = TRUE;
	memset(alphaDesc.BlendEnable+1, FALSE, 7*sizeof(BOOL));
	alphaDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	alphaDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	alphaDesc.BlendOp = D3D10_BLEND_OP_ADD;
	alphaDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
	alphaDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	alphaDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	memset(alphaDesc.RenderTargetWriteMask, D3D10_COLOR_WRITE_ENABLE_ALL, 8*sizeof(UINT8));
	device->CreateBlendState(&alphaDesc, &blendStates[BM_AlphaBlend]);

	D3D10_BLEND_DESC addDesc;
	addDesc.AlphaToCoverageEnable = FALSE;
	addDesc.BlendEnable[0] = TRUE;
	memset(addDesc.BlendEnable+1, FALSE, 7*sizeof(BOOL));
	addDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	addDesc.DestBlend = D3D10_BLEND_ONE;	
	addDesc.BlendOp = D3D10_BLEND_OP_ADD;
	addDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
	addDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	addDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	memset(addDesc.RenderTargetWriteMask, D3D10_COLOR_WRITE_ENABLE_ALL, 8*sizeof(UINT8));
	device->CreateBlendState(&addDesc, &blendStates[BM_Additive]);

	D3D10_BLEND_DESC subDesc;
	subDesc.AlphaToCoverageEnable = FALSE;
	subDesc.BlendEnable[0] = TRUE;
	memset(subDesc.BlendEnable+1, FALSE, 7*sizeof(BOOL));
	subDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	subDesc.DestBlend = D3D10_BLEND_ONE;
	subDesc.BlendOp = D3D10_BLEND_OP_REV_SUBTRACT;
	subDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
	subDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	subDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	memset(subDesc.RenderTargetWriteMask, D3D10_COLOR_WRITE_ENABLE_ALL, 8*sizeof(UINT8));
	device->CreateBlendState(&subDesc, &blendStates[BM_Subtractive]);

	D3D10_BLEND_DESC alphaMaskDesc;
	alphaMaskDesc.AlphaToCoverageEnable = FALSE;
	alphaMaskDesc.BlendEnable[0] = TRUE;
	memset(alphaMaskDesc.BlendEnable+1, FALSE, 7*sizeof(BOOL));
	alphaMaskDesc.SrcBlend = D3D10_BLEND_ZERO;
	alphaMaskDesc.DestBlend = D3D10_BLEND_SRC_ALPHA;
	alphaMaskDesc.BlendOp = D3D10_BLEND_OP_ADD;
	alphaMaskDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
	alphaMaskDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	alphaMaskDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	memset(alphaMaskDesc.RenderTargetWriteMask, D3D10_COLOR_WRITE_ENABLE_ALL, 8*sizeof(UINT8));
    device->CreateBlendState(&alphaMaskDesc, &blendStates[BM_AlphaMask]);

	D3D10_BLEND_DESC alphaPreMulDesc;
	alphaPreMulDesc.AlphaToCoverageEnable = FALSE;
	alphaPreMulDesc.BlendEnable[0] = TRUE;
	memset(alphaPreMulDesc.BlendEnable+1, FALSE, 7*sizeof(BOOL));
	alphaPreMulDesc.SrcBlend = D3D10_BLEND_ONE;
	alphaPreMulDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	alphaPreMulDesc.BlendOp = D3D10_BLEND_OP_ADD;
	alphaPreMulDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
	alphaPreMulDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	alphaPreMulDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	memset(alphaPreMulDesc.RenderTargetWriteMask, D3D10_COLOR_WRITE_ENABLE_ALL, 8*sizeof(UINT8));
    device->CreateBlendState(&alphaPreMulDesc, &blendStates[BM_AlphaPreMul]);

	// Create default (white) texture
	{
		D3D10_TEXTURE2D_DESC texDesc;
		texDesc.Width = 4;
		texDesc.Height = 4;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D10_USAGE_DEFAULT;
		texDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		unsigned int whitePixels[4*4];
		memset(whitePixels, -1, 4*4*sizeof(int));

		D3D10_SUBRESOURCE_DATA whiteStuff;
		whiteStuff.pSysMem = whitePixels;
		whiteStuff.SysMemPitch = 4*sizeof(int);
		whiteStuff.SysMemSlicePitch = 0;

		ID3D10Texture2D *pResource;
		D3D_VERIFY(device->CreateTexture2D(&texDesc, &whiteStuff, &pResource));

		ID3D10ShaderResourceView *pView;
		D3D_VERIFY(device->CreateShaderResourceView(pResource, NULL, &pView));

		pWhiteTex = new Texture2D("texWhite", 4, 4, pResource, pView);
	}
}

D3D::~D3D()
{
	delete pWhiteTex;

	for (size_t iBS = 0; iBS < MAX_BlendMode; ++iBS)
		SAFE_RELEASE(blendStates[iBS]);
	
	SAFE_RELEASE(depthStencilState[0]);
	SAFE_RELEASE(depthStencilState[1]);

	delete depthStencil;
	SAFE_RELEASE(rasterizerState);
	delete renderTargetBackBuffer;
}

void D3D::Clear(ID3D10RenderTargetView* renderTarget)
{
	const float RGBA[4] = { 0.f };
	device->ClearRenderTargetView(renderTarget, RGBA);
}

void D3D::BindBackbuffer(DepthStencil* depth)
{
	ID3D10RenderTargetView* view = renderTargetBackBuffer->GetRenderTargetView();
	device->OMSetRenderTargets(1, &view, depth ? depth->GetDepthStencilView() : nullptr);
}

void D3D::BindRenderTarget( RenderTarget* pixels, DepthStencil* depth)
{
	ASSERT(nullptr != pixels);
	ID3D10RenderTargetView* view = pixels->GetRenderTargetView();
	device->OMSetRenderTargets(1, &view, depth ? depth->GetDepthStencilView() : nullptr);
}

void D3D::BindRenderTargetTexture3D(Texture3D* pixels, int sliceIndex)
{
	ASSERT(nullptr != pixels && sliceIndex >= 0);
	ID3D10RenderTargetView* view = pixels->GetRenderTargetView(sliceIndex);
	device->OMSetRenderTargets(1, &view, nullptr); // No depth-stencil.
}

ID3D10Buffer* D3D::CreateVertexBuffer(int numBytes, const void* initialData, bool isDynamic)
{
	ASSERT(numBytes > 0);

	D3D10_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = (isDynamic) ? D3D10_USAGE_DYNAMIC : D3D10_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = numBytes;
	bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = (isDynamic) ? D3D10_CPU_ACCESS_WRITE : 0;
	bufferDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA data;
	data.pSysMem = initialData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	ID3D10Buffer* buffer = nullptr;
	const HRESULT hRes = device->CreateBuffer(&bufferDesc, (initialData == nullptr) ? nullptr : &data, &buffer);
	D3D_ASSERT(hRes);
	
	return buffer;
}

ID3D10Buffer* D3D::CreateIndexBuffer(int numIndices, const void* initialData, bool isDynamic)
{
	ASSERT(numIndices > 0);

	D3D10_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = (isDynamic) ? D3D10_USAGE_DYNAMIC : D3D10_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = numIndices * sizeof(DWORD);
	bufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = (isDynamic) ? D3D10_CPU_ACCESS_WRITE : 0;
	bufferDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA data;
	if (false == isDynamic)
	{
		data.pSysMem = initialData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
	}

	ID3D10Buffer* buffer = nullptr;
	const HRESULT hRes = device->CreateBuffer(&bufferDesc, (true == isDynamic) ? nullptr : &data, &buffer);
	D3D_ASSERT(hRes);

	return buffer;
}

ID3D10InputLayout* D3D::CreateInputLayout(
	const D3D10_INPUT_ELEMENT_DESC* description,
	int numDescriptionItems,
	unsigned char* signatureIA, int signatureIALength)
{
	ASSERT(nullptr != description);

	ID3D10InputLayout* layout = nullptr;
	const HRESULT hRes = device->CreateInputLayout(
		description, 
		numDescriptionItems, 
		signatureIA, 
		signatureIALength,
		&layout);
	D3D_ASSERT(hRes);

	return layout;
}

ID3D10Effect* D3D::CreateEffect(const unsigned char* compiledEffect, int compiledEffectLength)
{
	ASSERT(nullptr != compiledEffect && compiledEffectLength > 0);

	ID3D10Effect* effect = nullptr;
	const HRESULT hRes = D3D10CreateEffectFromMemory(
		(void*) compiledEffect, compiledEffectLength,
		0, device, nullptr, &effect);
	D3D_ASSERT(hRes);

	return effect;
}

RenderTarget* D3D::CreateRenderTarget(int shResinkFactor, DXGI_FORMAT format, bool multiSample)
{
	ASSERT(shResinkFactor >= 1);

	// Create texture
	D3D10_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = sceneVP.Width/shResinkFactor;
	desc.Height = sceneVP.Height/shResinkFactor;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = multiSample ? D3D_ANTIALIAS_NUM_SAMPLES : 1;
	desc.SampleDesc.Quality = multiSample ? D3D_ANTIALIAS_QUALITY : 0;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;

	ID3D10Texture2D* texture = nullptr;
	HRESULT hRes = device->CreateTexture2D(&desc, NULL, &texture);
	D3D_ASSERT(hRes);

	// Create view
	D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
	memset(&rtDesc, 0, sizeof(rtDesc));
	rtDesc.Format = format;
	rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DMS; // Multi-sampled!

	ID3D10RenderTargetView* renderTargetView = nullptr;

	// FIXME: if I do this my loading bar disappears!
//	hRes = device->CreateRenderTargetView(texture, (false == multiSample) ? nullptr : &rtDesc, &renderTargetView);
	hRes = device->CreateRenderTargetView(texture, nullptr, &renderTargetView);
	D3D_ASSERT(hRes);

	// Create shader view
	D3D10_SHADER_RESOURCE_VIEW_DESC shDesc;
	memset(&shDesc, 0, sizeof(shDesc));
	shDesc.Format = format;
	shDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DMS; // Multi-sampled!

	ID3D10ShaderResourceView* shaderResourceView = nullptr;

	// FIXME: if I do this my loading bar disappears!
//	hRes = device->CreateShaderResourceView(texture, (false == multiSample) ? nullptr : &shDesc, &shaderResourceView);
	hRes = device->CreateShaderResourceView(texture, nullptr, &shaderResourceView);
	D3D_ASSERT(hRes);

	return new RenderTarget(format, texture, renderTargetView, shaderResourceView);

	// FIXME: if it cops out anywhere here by way of throw we're obviously potentially leaking.
}

ID3D10Texture2D* D3D::CreateIntermediateCPUTarget(DXGI_FORMAT format)
{
	D3D10_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = sceneVP.Width;
	desc.Height = sceneVP.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D10_USAGE_STAGING;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
	desc.BindFlags = 0;

	ID3D10Texture2D* texture = nullptr;
	const HRESULT hRes = device->CreateTexture2D(&desc, nullptr, &texture);
	D3D_ASSERT(hRes);

	return texture;
}

DepthStencil* D3D::CreateDepthStencil(bool multiSample)
{
	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = backVP.Width;
	descDepth.Height = backVP.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = multiSample ? D3D_ANTIALIAS_NUM_SAMPLES : 1;
	descDepth.SampleDesc.Quality = multiSample ? D3D_ANTIALIAS_QUALITY : 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	// Create depth stencil
	ID3D10Texture2D* texture = nullptr;
	HRESULT hRes = device->CreateTexture2D(&descDepth, nullptr, &texture);
	D3D_ASSERT(hRes);

	// And it's view
	ID3D10DepthStencilView* view = nullptr;

	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	
	hRes = device->CreateDepthStencilView(texture, nullptr /* FIXME: why not &descDSV? */, &view);
	D3D_ASSERT(hRes);

	return new DepthStencil(texture, view);

	// FIXME: if it cops out anywhere here by way of throw we're obviously potentially leaking.
}

Texture2D* D3D::CreateTexture2D(const std::string& name, int width, int height, bool requiresGammaCorrection)
{
	D3D10_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = desc.ArraySize = 1; // Only a single mip
	desc.Format = requiresGammaCorrection ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D10_USAGE_DYNAMIC;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	ID3D10Texture2D* texture = nullptr;
	HRESULT hRes = device->CreateTexture2D(&desc, nullptr, &texture);
	D3D_ASSERT(hRes);

	ID3D10ShaderResourceView* view = nullptr;
	hRes = device->CreateShaderResourceView(texture, nullptr, &view);
	D3D_ASSERT(hRes);

	return new Texture2D(name, width, height, texture, view);

	// FIXME: if it cops out anywhere here by way of throw we're obviously potentially leaking.
}

Texture3D* D3D::CreateTexture3D(const std::string& name, int width, int height, int depth)
{
	D3D10_TEXTURE3D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.Depth = depth;
	desc.MipLevels = 1; // Only a single mip
	desc.Format = DXGI_FORMAT_R16G16_FLOAT;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE; 
	// ^^ All our 3d textures are render targets too!

	ID3D10Texture3D* texture = nullptr;
	HRESULT hRes = device->CreateTexture3D(&desc, nullptr, &texture);
	D3D_ASSERT(hRes);

	// Create view for sampling
	ID3D10ShaderResourceView* view = nullptr;
	hRes = device->CreateShaderResourceView(texture, nullptr, &view);
	D3D_ASSERT(hRes);

	// Create N views for rendering to
	ID3D10RenderTargetView** rtViews = new ID3D10RenderTargetView*[depth];

	for (int iSlice = 0; iSlice < depth; ++iSlice)
	{
		D3D10_RENDER_TARGET_VIEW_DESC rtv;
		rtv.Format = desc.Format;
		rtv.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE3D;
		rtv.Texture3D.MipSlice = 0;
		rtv.Texture3D.WSize = 1;
		rtv.Texture3D.FirstWSlice = iSlice;

		ID3D10RenderTargetView* renderTargetView = nullptr;
		hRes = device->CreateRenderTargetView(texture, &rtv, &renderTargetView);
		D3D_ASSERT(hRes);

		rtViews[iSlice] = renderTargetView;
	}

	return new Texture3D(name, width, height, depth, texture, view, rtViews);

	// FIXME: if it cops out anywhere here by way of throw we're obviously potentially leaking.
}

bool D3D::CompileEffect(const unsigned char* effectAscii, int effectAsciiSize, unsigned char** outCompiledEffectBuffer, int* outCompiledEffectLength)
{	
	ID3DBlob* shader = nullptr;
	ID3DBlob* errors = nullptr;

	const HRESULT hRes = D3DCompile(
		effectAscii,
		effectAsciiSize,
		NULL, // name
		NULL, // defines		
		NULL, // includes
		NULL, // entrypoint
		"fx_4_0", // target
		D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_SKIP_VALIDATION, // flags
		0, // effect compiler flags
		&shader,
		&errors);

	if (S_OK != hRes)
	{
		if (nullptr != errors)
		{
			char* errorMsg = reinterpret_cast<char*>(errors->GetBufferPointer());
			D3D_ASSERT_MSG(hRes, errorMsg);
		}

		return false;
	}

	ASSERT(shader->GetBufferSize() > 0);
	*outCompiledEffectBuffer = new unsigned char[shader->GetBufferSize()];
	ASSERT(*outCompiledEffectBuffer != NULL);
	memcpy(*outCompiledEffectBuffer, (unsigned char*)shader->GetBufferPointer(), (unsigned int)shader->GetBufferSize());
	*outCompiledEffectLength = (int)shader->GetBufferSize();

	shader->Release();
	return true;
}


} // namespace Pimp
