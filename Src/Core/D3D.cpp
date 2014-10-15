#include "D3D.h"
#include <shared/shared.h>
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "Configuration.h"

#include "D3DCompiler.h"

namespace Pimp
{

D3D* gD3D = NULL;


D3D::D3D(ID3D10Device1 *device, IDXGISwapChain* swapchain) : 
	device(device), swapchain(swapchain)
,	rasterizerState(nullptr)
,	renderTargetBackBuffer(nullptr)
,	depthStencil(nullptr)
,	depthStencilState(nullptr)
,	pWhiteTex(nullptr)
{
	memset(blendStates, 0, MAX_BlendMode*sizeof(BlendMode));

	HRESULT hr;

	const Configuration::DisplayMode& mode = Configuration::Instance()->GetDisplayMode();
	viewWidth = mode.width;
	viewHeight = mode.height;

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

	// Retrieve backbuffer.
	ID3D10Texture2D* backbuffer;
	hr = swapchain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backbuffer);
	D3D_ASSERT(hr);
	ASSERT(backbuffer != NULL);

	// Create render target view for backbuffer.
	ID3D10RenderTargetView* renderTargetViewBackbuffer;
	hr = device->CreateRenderTargetView(backbuffer, NULL, &renderTargetViewBackbuffer);
	D3D_ASSERT(hr);
	ASSERT(renderTargetViewBackbuffer != NULL);

	renderTargetBackBuffer = new RenderTarget(PIMP_BACKBUFFER_FORMAT_GAMMA, backbuffer, renderTargetViewBackbuffer, NULL);

	// Resort to triangle lists.
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Rasterizer state.
	D3D10_RASTERIZER_DESC rasterDesc;
	memset(&rasterDesc, 0, sizeof(rasterDesc));
	rasterDesc.FillMode = D3D10_FILL_SOLID;
	rasterDesc.CullMode = D3D10_CULL_BACK;
	//rasterDesc.FrontCounterClockwise = FALSE;
	//rasterDesc.DepthBias = 0;
	//rasterDesc.DepthBiasClamp = 0;
	//rasterDesc.SlopeScaledDepthBias = 0;
	rasterDesc.DepthClipEnable = TRUE;
	//rasterDesc.ScissorEnable = FALSE;
	rasterDesc.MultisampleEnable = TRUE;
	//rasterDesc.AntialiasedLineEnable = FALSE;
	
	rasterizerState = NULL;
	hr = device->CreateRasterizerState(&rasterDesc, &rasterizerState);
	D3D_ASSERT(hr);

	device->RSSetState(rasterizerState);

	// Create depth-stencil
	depthStencil = CreateDepthStencil(true);

	// Depth-stencil state
	D3D10_DEPTH_STENCIL_DESC descDepthS;
	memset(&descDepthS, 0, sizeof(descDepthS));
	descDepthS.DepthEnable = true;
	descDepthS.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	descDepthS.DepthFunc = D3D10_COMPARISON_LESS;

	depthStencilState = NULL;
	hr = device->CreateDepthStencilState(&descDepthS, &depthStencilState);
	D3D_ASSERT(hr);
	ASSERT(depthStencilState != NULL);

	device->OMSetDepthStencilState(depthStencilState, 0);

	// Bind our backbuffer and depth stencil by default
	BindBackbuffer(depthStencil);

	// For letterboxing..
	float renderAspectRatio = Configuration::Instance()->GetRenderAspectRatio();
	renderScale = Vector2(1.f, renderAspectRatio);


	// Aspect ratio adjust.
	//

	// define full & adjusted (16:9) viewport
	m_fullVP.TopLeftX = 0;
	m_fullVP.TopLeftY = 0;
	m_fullVP.Width = viewWidth;
	m_fullVP.Height = viewHeight;
	m_fullVP.MinDepth = 0.f;
	m_fullVP.MaxDepth = 1.f;

	const float fullAspectRatio = (float) m_fullVP.Width / m_fullVP.Height;
	const float desired_aspect_ratio_on_screen = Configuration::Instance()->GetRenderAspectRatio();
	const float renderableAmount = fullAspectRatio / desired_aspect_ratio_on_screen;

	unsigned int xResAdj, yResAdj;

	xResAdj = m_fullVP.Width;
	yResAdj = (unsigned int) (m_fullVP.Height*renderableAmount);

	renderScale.x = 1.0f;
	renderScale.y = renderableAmount;
	
	m_adjVP.Width = xResAdj;
	m_adjVP.Height = yResAdj;
	m_adjVP.TopLeftX = (m_fullVP.Width-xResAdj)/2;
	m_adjVP.TopLeftY = (m_fullVP.Height-yResAdj)/2;
	m_adjVP.MinDepth = 0.f;
	m_adjVP.MaxDepth = 1.f;

	// Readjust to aspect corrected VP
	device->RSSetViewports(1, &m_adjVP);

	// Blend states
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

	// create default (white) texture
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
		VERIFY(S_OK == device->CreateTexture2D(&texDesc, &whiteStuff, &pResource));

		ID3D10ShaderResourceView *pView;
		VERIFY(S_OK == device->CreateShaderResourceView(pResource, NULL, &pView));

		pWhiteTex = new Texture2D("texWhite", 4, 4, pResource, pView);
	}
}


D3D::~D3D()
{
	delete pWhiteTex;

	for (size_t iBS = 0; iBS < MAX_BlendMode; ++iBS)
		if (nullptr != blendStates[iBS]) blendStates[iBS]->Release();
	
	if (nullptr != depthStencilState) depthStencilState->Release();
	delete depthStencil;
	if (nullptr != rasterizerState) rasterizerState->Release();
	delete renderTargetBackBuffer;
}


void D3D::Clear(ID3D10RenderTargetView* renderTarget, float R, float G, float B, float A)
{
	// Clear entire screen
	device->RSSetViewports(1, &m_fullVP);
	const float RGBA[4] = { R, G, B, A };
	device->ClearRenderTargetView(renderTarget, RGBA);

	// Bind adjusted (aspect) viewport
	device->RSSetViewports(1, &m_adjVP);

//	ClearDepthStencil();
}


void D3D::Flip()
{
	HRESULT hr = swapchain->Present(0,0);
	D3D_ASSERT(hr);
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


	ID3D10Buffer* buffer = NULL;
	HRESULT hr = device->CreateBuffer(&bufferDesc, (initialData == NULL) ? nullptr : &data, &buffer);
	D3D_ASSERT(hr);
	ASSERT(buffer != NULL);

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

	ID3D10Buffer* buffer = NULL;
	HRESULT hr = device->CreateBuffer(&bufferDesc, (true == isDynamic) ? nullptr : &data, &buffer);
	D3D_ASSERT(hr);
	ASSERT(buffer != NULL);

	return buffer;
}


void D3D::BindVertexBuffer(int slot, ID3D10Buffer* buffer, unsigned int stride)
{
	unsigned int offset = 0;
	device->IASetVertexBuffers(slot, 1, &buffer, &stride, &offset);
}


void D3D::BindIndexBuffer(ID3D10Buffer* buffer)
{
	device->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
}


void D3D::BindInputLayout(ID3D10InputLayout* layout)
{
	device->IASetInputLayout(layout);
}


void D3D::DrawTriQuad(DWORD offset)
{
	device->Draw(6, offset);
}


ID3D10Effect* D3D::CreateEffect(const unsigned char* compiledEffect, int compiledEffectLength)
{
	ID3D10Effect* effect = NULL;

	HRESULT hr = D3D10CreateEffectFromMemory(
		(void*)compiledEffect, compiledEffectLength,
		0, device, NULL, &effect);

	D3D_ASSERT(hr);
	ASSERT(effect != NULL);

	return effect;
}


ID3D10InputLayout* D3D::CreateInputLayout(
	const D3D10_INPUT_ELEMENT_DESC* description,
	int numDescriptionItems,
	unsigned char* signatureIA, int signatureIALength)
{
	ID3D10InputLayout* layout = NULL;

	HRESULT hr = device->CreateInputLayout(
		description, numDescriptionItems, signatureIA, signatureIALength,
		&layout);
	
	D3D_ASSERT(hr);
	ASSERT(layout != NULL);

	return layout;
}


RenderTarget* D3D::CreateRenderTarget( int viewportShrinkFactor, DXGI_FORMAT format, bool multiSample )
{
	ASSERT(viewportShrinkFactor >= 1);

	// Create texture
	D3D10_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.Width = viewWidth/viewportShrinkFactor;
	desc.Height = viewHeight/viewportShrinkFactor;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = multiSample ? D3D_ANTIALIAS_NUM_SAMPLES : 1;
	desc.SampleDesc.Quality = multiSample ? D3D_ANTIALIAS_QUALITY : 0;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;

	ID3D10Texture2D* texture = NULL;
	HRESULT hr = device->CreateTexture2D(&desc, NULL, &texture);
	D3D_ASSERT(hr);
	ASSERT(texture != NULL);

	// Create rendertarget-view
	//D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
	//FillSomeMemory(&rtDesc, sizeof(rtDesc), 0);
	//rtDesc.Format = format;
	//rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DMS; // Multi-sampled!

	ID3D10RenderTargetView* renderTargetView = NULL;
	hr = device->CreateRenderTargetView(texture, NULL/*&rtDesc*/, &renderTargetView);
	D3D_ASSERT(hr);
	ASSERT(renderTargetView != NULL);	

	// Create shaderresource view
	//D3D10_SHADER_RESOURCE_VIEW_DESC shDesc;
	//FillSomeMemory(&shDesc, sizeof(shDesc), 0);
	//shDesc.Format = format;
	//shDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DMS; // Multi-sampled!

	ID3D10ShaderResourceView* shaderResourceView = NULL;
	hr = device->CreateShaderResourceView(texture, NULL/*&shDesc*/, &shaderResourceView);
	D3D_ASSERT(hr);
	ASSERT(shaderResourceView != NULL);

	return new RenderTarget(format, texture, renderTargetView, shaderResourceView);
}


ID3D10Texture2D* D3D::CreateIntermediateCPUTarget(DXGI_FORMAT format)
{
	// Create texture
	D3D10_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.Width = viewWidth;
	desc.Height = viewHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D10_USAGE_STAGING;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
	desc.BindFlags = 0;

	ID3D10Texture2D* texture = NULL;
	HRESULT hr = device->CreateTexture2D(&desc, NULL, &texture);
	D3D_ASSERT(hr);
	ASSERT(texture != NULL);

	return texture;
}

void D3D::CopyTextures(ID3D10Texture2D* dest, ID3D10Texture2D* src)
{
	device->CopyResource(dest, src);
}

void D3D::BindBackbuffer(DepthStencil* depth)
{
	ID3D10RenderTargetView* view = renderTargetBackBuffer->GetRenderTargetView();
	device->OMSetRenderTargets(1, &view, depth ? depth->GetDepthStencilView() : NULL);
}

void D3D::GetViewportSize(int* width, int* height)
{
	*width = viewWidth;
	*height = viewHeight;
}

void D3D::ClearBackBuffer()
{
	Clear(renderTargetBackBuffer->GetRenderTargetView(), 0.f, 0.f, 0.f, 1.f);
}

void D3D::ResolveMultiSampledRenderTarget( ID3D10Texture2D* dest, ID3D10Texture2D* source, DXGI_FORMAT format )
{
	device->ResolveSubresource(dest, 0, source, 0, format);
}

DepthStencil* D3D::CreateDepthStencil(bool multiSample)
{
	// Depth-stencil
	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = viewWidth;
	descDepth.Height = viewHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = multiSample ? D3D_ANTIALIAS_NUM_SAMPLES : 1;
	descDepth.SampleDesc.Quality = multiSample ? D3D_ANTIALIAS_QUALITY : 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	ID3D10Texture2D* texture = NULL;
	HRESULT hr = device->CreateTexture2D( &descDepth, NULL, &texture);
	D3D_ASSERT(hr);
	ASSERT(texture != NULL);

	// Depth-stencil view
	ID3D10DepthStencilView* view = NULL;

	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	
	hr = device->CreateDepthStencilView(texture, NULL, &view);
	D3D_ASSERT(hr);
	ASSERT(view != NULL);

	return new DepthStencil(texture, view);
}


void D3D::BindRenderTarget( RenderTarget* pixels, DepthStencil* depth )
{
	ID3D10RenderTargetView* view = pixels->GetRenderTargetView();

	device->OMSetRenderTargets(1, &view, depth ? depth->GetDepthStencilView() : NULL);
}


void D3D::BindRenderTargetTexture3D(Texture3D* pixels, int sliceIndex)
{
	ID3D10RenderTargetView* view = pixels->GetRenderTargetView(sliceIndex);

	device->OMSetRenderTargets(1, &view, NULL);
}


void D3D::ClearDepthStencil()
{
	device->ClearDepthStencilView(depthStencil->GetDepthStencilView(), D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0, 0 );
}

DepthStencil* D3D::GetDefaultDepthStencil() const
{
	return depthStencil;
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

	ID3D10Texture2D* texture = NULL;
	HRESULT hr = device->CreateTexture2D(&desc, NULL, &texture);
	D3D_ASSERT(hr);

	ID3D10ShaderResourceView* view = NULL;
	hr = device->CreateShaderResourceView(texture, NULL/*&shDesc*/, &view);
	D3D_ASSERT(hr);
	ASSERT(view != NULL);

	return new Texture2D(name, width, height, texture, view);
}


Texture3D* D3D::CreateTexture3D(const std::string& name, int width, int height, int depth)
{
	D3D10_TEXTURE3D_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.Width = width;
	desc.Height = height;
	desc.Depth = depth;
	desc.MipLevels = 1; // Only a single mip
	desc.Format = DXGI_FORMAT_R16G16_FLOAT; //DXGI_FORMAT_R32_FLOAT; //DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE; // <-- all our 3d textures are render targets too!

	ID3D10Texture3D* texture = NULL;
	HRESULT hr = device->CreateTexture3D(&desc, NULL, &texture);
	D3D_ASSERT(hr);

	// Create view for sampling
	ID3D10ShaderResourceView* view = NULL;
	hr = device->CreateShaderResourceView(texture, NULL/*&shDesc*/, &view);
	D3D_ASSERT(hr);
	ASSERT(view != NULL);

	// Create N views for rendering to
	ID3D10RenderTargetView** rtviews = new ID3D10RenderTargetView*[depth];

	for (int i=0; i<depth; ++i)
	{
		D3D10_RENDER_TARGET_VIEW_DESC rtv;
		rtv.Format = desc.Format;
		rtv.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE3D;
		rtv.Texture3D.MipSlice = 0;
		rtv.Texture3D.WSize = 1;
		rtv.Texture3D.FirstWSlice = i;

		ID3D10RenderTargetView* renderTargetView = NULL;
		hr = device->CreateRenderTargetView(texture, &rtv, &renderTargetView);
		D3D_ASSERT(hr);
		ASSERT(renderTargetView != NULL);

		rtviews[i] = renderTargetView;
	}

	return new Texture3D(name, width, height, depth, texture, view, rtviews);
}


void D3D::SetBlendMode(BlendMode blendMode)
{
	ASSERT((int)blendMode < MAX_BlendMode);

	device->OMSetBlendState(blendStates[(int)blendMode], NULL, 0xFFFFFFFF);
}


bool D3D::CompileEffect(const unsigned char* effectAscii, int effectAsciiSize, unsigned char** outCompiledEffectBuffer, int* outCompiledEffectLength)
{	
	ID3DBlob* shader = NULL;
	ID3DBlob* errors = NULL;

	// from http://msdn.microsoft.com/en-us/library/windows/desktop/dd607324%28v=vs.85%29.aspx
	HRESULT hr = D3DCompile(
		effectAscii,
		effectAsciiSize,
		NULL, // name
		NULL, // defines		
		NULL, // includes
		NULL, // entrypoint
		"fx_4_0", // target
		D3DCOMPILE_OPTIMIZATION_LEVEL3|D3DCOMPILE_SKIP_VALIDATION, // flags
		0, // effect compiler flags
		&shader,
		&errors);

	char* _errors = (errors != NULL) ? (char*)errors->GetBufferPointer() : NULL;
	D3D_ASSERT_MSG(hr, _errors);
	if (hr != S_OK)
		return false;

	ASSERT(shader->GetBufferSize() > 0);
	*outCompiledEffectBuffer = new unsigned char[shader->GetBufferSize()];
	ASSERT(*outCompiledEffectBuffer != NULL);
	memcpy(*outCompiledEffectBuffer, (unsigned char*)shader->GetBufferPointer(), (unsigned int)shader->GetBufferSize());
	*outCompiledEffectLength = (int)shader->GetBufferSize();

	return true;
}


} // namespace Pimp
