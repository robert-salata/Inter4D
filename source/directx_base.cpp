#include "directx_base.h"

DirectBase::DirectBase(int screen_width, int screen_height, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear, bool mode_3d)
{	
	vsync_enabled = vsync;

	IDXGIFactory* factory;
	HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	IDXGIAdapter* adapter;
	result = factory->EnumAdapters(0, &adapter);
	IDXGIOutput* adapterOutput;
	result = adapter->EnumOutputs(0, &adapterOutput);
	unsigned int numModes;
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	int numerator = 0, denominator = 1;
	for(int i = 0; i < numModes; i++)
		if(displayModeList[i].Width == (unsigned int)screen_width && displayModeList[i].Height == (unsigned int)screen_height)
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
		}
	DXGI_ADAPTER_DESC adapterDesc;
	result = adapter->GetDesc(&adapterDesc);
	video_card_memory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	unsigned int stringLength;
	int error = wcstombs_s(&stringLength, video_card_description, 128, adapterDesc.Description, 128);
	delete [] displayModeList;
	adapterOutput->Release();
	adapter->Release();
	factory->Release();

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = screen_width;
    swapChainDesc.BufferDesc.Height = screen_height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = vsync_enabled ? numerator : 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = vsync_enabled ? denominator : 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = !fullscreen;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, 
		&swap_chain, &device, NULL, &device_context);

	ID3D11Texture2D* backBufferPtr;
	result = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	result = device->CreateRenderTargetView(backBufferPtr, NULL, &render_target_view);
	backBufferPtr->Release();
	backBufferPtr = 0;

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = screen_width;
	depthBufferDesc.Height = screen_height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &depth_stencil_buffer);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = mode_3d;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS; //D3D11_COMPARISON_LESS
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	result = device->CreateDepthStencilState(&depthStencilDesc, &depth_stencil_state);
	device_context->OMSetDepthStencilState(depth_stencil_state, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	result = device->CreateDepthStencilView(depth_stencil_buffer, &depthStencilViewDesc, &depth_stencil_view);
	device_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

	D3D11_RASTERIZER_DESC front_rasterizer_desc;
	front_rasterizer_desc.AntialiasedLineEnable = false;
	front_rasterizer_desc.CullMode = D3D11_CULL_BACK;
	front_rasterizer_desc.DepthBias = 0;
	front_rasterizer_desc.DepthBiasClamp = 0.0f;
	front_rasterizer_desc.DepthClipEnable = true;
	front_rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	front_rasterizer_desc.FrontCounterClockwise = false;
	front_rasterizer_desc.MultisampleEnable = false;
	front_rasterizer_desc.ScissorEnable = false;
	front_rasterizer_desc.SlopeScaledDepthBias = 0.0f;
	result = device->CreateRasterizerState(&front_rasterizer_desc, &front_rasterizer);
	
	D3D11_RASTERIZER_DESC back_rasterizer_desc;
	back_rasterizer_desc.AntialiasedLineEnable = false;
	back_rasterizer_desc.CullMode = D3D11_CULL_FRONT;
	back_rasterizer_desc.DepthBias = 0;
	back_rasterizer_desc.DepthBiasClamp = 0.0f;
	back_rasterizer_desc.DepthClipEnable = true;
	back_rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	back_rasterizer_desc.FrontCounterClockwise = false;
	back_rasterizer_desc.MultisampleEnable = false;
	back_rasterizer_desc.ScissorEnable = false;
	back_rasterizer_desc.SlopeScaledDepthBias = 0.0f;
	result = device->CreateRasterizerState(&back_rasterizer_desc, &back_rasterizer);

	D3D11_RASTERIZER_DESC both_rasterizer_desc;
	both_rasterizer_desc.AntialiasedLineEnable = false;
	both_rasterizer_desc.CullMode = D3D11_CULL_NONE;
	both_rasterizer_desc.DepthBias = 0;
	both_rasterizer_desc.DepthBiasClamp = 0.0f;
	both_rasterizer_desc.DepthClipEnable = true;
	both_rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	both_rasterizer_desc.FrontCounterClockwise = false;
	both_rasterizer_desc.MultisampleEnable = false;
	both_rasterizer_desc.ScissorEnable = false;
	both_rasterizer_desc.SlopeScaledDepthBias = 0.0f;
	result = device->CreateRasterizerState(&both_rasterizer_desc, &both_rasterizer);

	D3D11_RASTERIZER_DESC frame_rasterizer_desc = front_rasterizer_desc;
	frame_rasterizer_desc.AntialiasedLineEnable = false;
	frame_rasterizer_desc.CullMode = D3D11_CULL_NONE;
	frame_rasterizer_desc.DepthBias = 0;
	frame_rasterizer_desc.DepthBiasClamp = 0.0f;
	frame_rasterizer_desc.DepthClipEnable = true;
	frame_rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	frame_rasterizer_desc.FrontCounterClockwise = false;
	frame_rasterizer_desc.MultisampleEnable = false;
	frame_rasterizer_desc.ScissorEnable = false;
	frame_rasterizer_desc.SlopeScaledDepthBias = 0.0f;
	result = device->CreateRasterizerState(&frame_rasterizer_desc, &frame_rasterizer);

	D3D11_BLEND_DESC blending = { 0 };
	blending.AlphaToCoverageEnable = false;
	blending.IndependentBlendEnable = false;
	blending.RenderTarget[0].BlendEnable = true;
	blending.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blending.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blending.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blending.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blending.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blending.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blending.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	result = device->CreateBlendState(&blending, &blend_state);
	float blend_factor[4] = { 0, 0, 0, 0 };
	device_context->OMSetBlendState(blend_state, blend_factor, 0xffffffff);

	D3D11_VIEWPORT viewport;
    viewport.Width = (float)screen_width;
    viewport.Height = (float)screen_height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    device_context->RSSetViewports(1, &viewport);

	if (mode_3d)
	{
		float fieldOfView = (float)D3DX_PI / 4.0f;
		float screenAspect = (float)screen_width / (float)screen_height;
		D3DXMatrixPerspectiveFovLH(&projection_matrix, fieldOfView, screenAspect, screenNear, screenDepth);
		D3DXMatrixIdentity(&world_matrix);
		D3DXMatrixOrthoLH(&ortho_matrix, (float)screen_width, (float)screen_height, screenNear, screenDepth);
	}
	else
	{
		D3DXMatrixPerspectiveFovLH(&projection_matrix, (float)D3DX_PI / 4.0f, (float)screen_width / (float)screen_height, 0, 1);
		D3DXMatrixIdentity(&world_matrix);
		D3DXMatrixIdentity(&ortho_matrix);
		D3DXMatrixOrthoLH(&ortho_matrix, (float)screen_width, (float)screen_height, screenNear, screenDepth);
	}
}

DirectBase::~DirectBase()
{
	swap_chain->SetFullscreenState(false, NULL);
	front_rasterizer->Release();
	back_rasterizer->Release();
	frame_rasterizer->Release();
	both_rasterizer->Release();
	blend_state->Release();
	depth_stencil_view->Release();
	depth_stencil_state->Release();
	depth_stencil_buffer->Release();
	render_target_view->Release();
	device_context->Release();
	device->Release();
	swap_chain->Release();
}

void DirectBase::set_rasterization(RasterizationType type)
{
	switch (type)
	{
		case BACK:
			device_context->RSSetState(back_rasterizer);
			return;
		case FRONT:
			device_context->RSSetState(front_rasterizer);
			return;
		case BOTH:
			device_context->RSSetState(both_rasterizer);
			return;
		case FRAME:
			device_context->RSSetState(frame_rasterizer);
			return;
	}
}

void DirectBase::begin_scene(float red, float green, float blue, float alpha)
{
	float color[4] = { red, green, blue, alpha };
	device_context->ClearRenderTargetView(render_target_view, color);
	device_context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DirectBase::end_scene()
{
	if(vsync_enabled)
		swap_chain->Present(1, 0);
	else
		swap_chain->Present(0, 0);
}

ID3D11Device* DirectBase::get_device()
{
	return device;
}

ID3D11DeviceContext* DirectBase::get_device_context()
{
	return device_context;
}

void DirectBase::get_projection_matrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = projection_matrix;
}

void DirectBase::get_world_matrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = world_matrix;
}

void DirectBase::get_ortho_matrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = ortho_matrix;
}

void DirectBase::get_video_card_info(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, video_card_description);
	memory = video_card_memory;
}