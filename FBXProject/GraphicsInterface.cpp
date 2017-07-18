#include "stdafx.h"
#include "GraphicsInterface.h"
#include "DirectXHelper.h"
#include <fstream>
#include "ProjectDataTypes.h"
#include <d3dcompiler.h>
#pragma comment (lib, "d3dcompiler.lib")

namespace csoReader {
	std::vector<byte> readCSOFile(char* fileName) {
		std::ifstream filestream;
		filestream.open(fileName, std::ifstream::binary | std::ifstream::in);
		if (filestream.fail()) {
			std::cout << "File failed." << std::endl;
		}
		std::vector<byte> csoFile;
		//filestream.open(fileName,std::fstream::binary || std::fstream::in);
		char holder;
		int result = filestream.is_open();
		if (result == false) {
			std::cout << "File "<< fileName <<" is not open." << std::endl;
		}
		while (filestream.get(holder)) {
			csoFile.push_back(holder);
		}
		std::cout << fileName << " has been read in." << std::endl;
		filestream.close();
		std::cout << fileName << " has been closed." << std::endl;
		return csoFile;
	}
}

GraphicsInterface* GraphicsInterface::instance = nullptr;

GraphicsInterface::GraphicsInterface() {
	instance = this;
	device = nullptr;
	devcon = nullptr;
	swapchain = nullptr;

	rasterizerState = nullptr;
	wireframeRasterizerState = nullptr;
	linearSamplerState = nullptr;
	anisotropicSamplerState = nullptr;

	dsState = nullptr;
	dsView = nullptr;
	dsBuffer = nullptr;

	rtv = nullptr;
	baseVertexShader = nullptr;
	baseVSInputLayout = nullptr;
	basePixelShader = nullptr;

	baseCubeBuffer = nullptr;
	baseCubeIndexBuffer = nullptr;
}

void GraphicsInterface::InitRasterizerState()
{
	//Set up Rasterizer
	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	//rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	std::cout << "\nFill rasterizer state initialized. Pointer to: " << rasterizerState << std::endl;
}

void GraphicsInterface::InitWireframeState()
{
	//Set up Rasterizer
	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	//rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	device->CreateRasterizerState(&rasterizerDesc, &wireframeRasterizerState);
	std::cout << "\nWireframe rasterizer state initialized. Pointer to: " << wireframeRasterizerState << std::endl;
}

void GraphicsInterface::InitLinearSampler()
{
	D3D11_SAMPLER_DESC linearSamplerDesc;
	linearSamplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	linearSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	linearSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	linearSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	linearSamplerDesc.MinLOD = -FLT_MAX;
	linearSamplerDesc.MaxLOD = FLT_MAX;
	linearSamplerDesc.MipLODBias = 0.0f;
	linearSamplerDesc.MaxAnisotropy = 1;
	linearSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	linearSamplerDesc.BorderColor[0] = 1.0f;
	linearSamplerDesc.BorderColor[1] = 1.0f;
	linearSamplerDesc.BorderColor[2] = 1.0f;
	linearSamplerDesc.BorderColor[3] = 1.0f;

	device->CreateSamplerState(&linearSamplerDesc, &linearSamplerState);
	std::cout << "\nLinear sampler state initialized. Pointer to: " << linearSamplerState << std::endl;
}

void GraphicsInterface::InitAnisoSampler()
{
	D3D11_SAMPLER_DESC anisotropicSamplerDesc;
	anisotropicSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	anisotropicSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	anisotropicSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	anisotropicSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	anisotropicSamplerDesc.MipLODBias = 0.0f;
	anisotropicSamplerDesc.MaxAnisotropy = 8;
	anisotropicSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	anisotropicSamplerDesc.BorderColor[0] = 0;
	anisotropicSamplerDesc.BorderColor[1] = 0;
	anisotropicSamplerDesc.BorderColor[2] = 0;
	anisotropicSamplerDesc.BorderColor[3] = 0;
	anisotropicSamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	anisotropicSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&anisotropicSamplerDesc, &anisotropicSamplerState);
	std::cout << "\nAnisotropic sampler state initialized. Pointer to: " << anisotropicSamplerState << std::endl;
}

void GraphicsInterface::InitDSState()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

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

	HRESULT result = device->CreateDepthStencilState(&depthStencilDesc, &dsState);
	std::cout << "\nDSState initialized. Pointer to: " << dsState << std::endl;
}

void GraphicsInterface::InitDSView()
{
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	HRESULT result = device->CreateDepthStencilView(dsBuffer, &depthStencilViewDesc, &dsView);
	std::cout << "\nDSV initialized. Pointer to: " << dsView << std::endl;
}

void GraphicsInterface::InitDSBuffer()
{
	DXGI_SWAP_CHAIN_DESC swchdesc;
	swapchain->GetDesc(&swchdesc);
	const uint32_t screenWidth = swchdesc.BufferDesc.Width;//1280u;//win->get_properties().width;
	const uint32_t screenHeight = swchdesc.BufferDesc.Height;//720u;//win->get_properties().height;

	D3D11_TEXTURE2D_DESC depthBufferDesc;

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	HRESULT result = device->CreateTexture2D(&depthBufferDesc, NULL, &dsBuffer);
	std::cout << "\nDepth buffer initialized. Pointer to: " << dsBuffer << std::endl;
}

void GraphicsInterface::InitRTV()
{
	if (swapchain == nullptr) {
		return;
	}
	ID3D11Texture2D *backbuffer;
	swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));
	device->CreateRenderTargetView(backbuffer, nullptr, &rtv);
	std::cout << "\nRTV initialized. Pointer to: " << rtv << std::endl;
}

void GraphicsInterface::InitViewPort()
{
	ID3D11Texture2D *backbuffer;
	swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));

	viewport = CD3D11_VIEWPORT(backbuffer, rtv);
	std::cout << "\nViewport initialized." << std::endl;
}

void GraphicsInterface::InitBaseVertexShader()
{
	/*ID3DBlob* pblob = nullptr;
	HRESULT result = D3DCompileFromFile(L"BaseVertexShader.hlsl", NULL, NULL, "main", "vs_4_0", NULL, NULL, &pblob, NULL);*/

	std::vector<byte> csoFile = csoReader::readCSOFile("BaseVertexShader.cso");
	HRESULT result = device->CreateVertexShader(&csoFile[0], csoFile.size(),NULL,&baseVertexShader);
	std::cout << "\nBase Vertex Shader initialized. Pointer to: " << baseVertexShader << std::endl;

	D3D11_INPUT_ELEMENT_DESC baseVertexDesc[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR0",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	std::cout << "\nBase Vertex Shader Input Layout: " << std::endl;
	std::cout << "Array Size: " << ARRAYSIZE(baseVertexDesc) << std::endl;
	std::cout << "csoFile location: " << (void*)&csoFile[0] << std::endl;
	std::cout << "Before CreateInputLayout call, vsIL: " << baseVSInputLayout << std::endl;
	result = device->CreateInputLayout(baseVertexDesc, ARRAYSIZE(baseVertexDesc), &csoFile[0], csoFile.size(), &baseVSInputLayout);
	std::cout << "Base Vertex Shader input layout initialized. Pointer to: " << baseVSInputLayout << std::endl;
//	ID3D11InputLayout *pInputLayout;
//	result = device->CreateInputLayout(baseVertexDesc, ARRAYSIZE(baseVertexDesc), &csoFile[0], csoFile.size(), &pInputLayout);
}

void GraphicsInterface::InitBasePixelShader()
{
	std::vector<byte> csoFile = csoReader::readCSOFile("BasePixelShader.cso");
	device->CreatePixelShader(&csoFile[0], csoFile.size(), NULL, &basePixelShader);
}

void GraphicsInterface::InitBaseCubeVertexBuffer()
{
	// Load mesh vertices. Each vertex has a position and a color.
	static const BasePosition baseCubeVertices[] =
	{
		{ DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) },
		{ DirectX::XMFLOAT3(-0.5f, -0.5f,  0.5f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT3(0.0f, 1.0f, 1.0f) },
		{ DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ DirectX::XMFLOAT3(0.5f, -0.5f,  0.5f), DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f) },
		{ DirectX::XMFLOAT3(0.5f,  0.5f, -0.5f), DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f) },
		{ DirectX::XMFLOAT3(0.5f,  0.5f,  0.5f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	};

	D3D11_SUBRESOURCE_DATA baseCubeVertexBufferData = { 0 };
	baseCubeVertexBufferData.pSysMem = baseCubeVertices;
	baseCubeVertexBufferData.SysMemPitch = 0;
	baseCubeVertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC baseCubeVertexBufferDesc(sizeof(baseCubeVertices), D3D11_BIND_VERTEX_BUFFER);
	device->CreateBuffer(
		&baseCubeVertexBufferDesc,
		&baseCubeVertexBufferData,
		&baseCubeBuffer
	);

	// Load mesh indices.
	static const unsigned short baseCubeIndices[] =
	{
		0,2,1, // -x
		1,2,3,

		4,5,6, // +x
		5,7,6,

		0,1,5, // -y
		0,5,4,

		2,6,7, // +y
		2,7,3,

		0,4,6, // -z
		0,6,2,

		1,3,7, // +z
		1,7,5,
	};

	baseCubeIndexCount = ARRAYSIZE(baseCubeIndices);

	D3D11_SUBRESOURCE_DATA baseCubeIndexBufferData = { 0 };
	baseCubeIndexBufferData.pSysMem = baseCubeIndices;
	baseCubeIndexBufferData.SysMemPitch = 0;
	baseCubeIndexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC baseCubeIndexBufferDesc(sizeof(baseCubeIndices), D3D11_BIND_INDEX_BUFFER);
	device->CreateBuffer(
		&baseCubeIndexBufferDesc,
		&baseCubeIndexBufferData,
		&baseCubeIndexBuffer
	);
}

void GraphicsInterface::InitializeD3D(HWND * hWnd)
{
	//create a struct to hold informationa bout the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	//clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	//fill the swap chain description struct
	scd.BufferCount = 1;								//one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//how wap chain is to be used
	scd.OutputWindow = *hWnd;							//the window to be used
	scd.SampleDesc.Count = 4;							//how many multisamples
	scd.Windowed = TRUE;								//windowed/full-screen mode

														//create a device, device context, and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,		//IDXGIAdapter
		D3D_DRIVER_TYPE_HARDWARE,			//D3D_DRIVER_TYPE
		NULL,								//HMODULE
		NULL,								//UINT Flags
		NULL,								//D3D_FEATURE_LEVEL
		NULL,								//UINT FeatureLevels
		D3D11_SDK_VERSION,					//SDKVersion
		&scd,								//DXGI_SWAP_CHAIN_DESC
		&swapchain,							//Pointer to swap chain pointer
		&device,							//Pointer to device pointer
		NULL,								//D3D_FEATURE_LEVEL
		&devcon);							//Pointer to device context

	//Take care of initialization inside the get functions
	/*
	if (rasterizerState == nullptr) {
		InitRasterizerState();
	}
	if (wireframeRasterizerState == nullptr) {
		InitWireframeState();
	}
	if (linearSamplerState == nullptr) {
		InitLinearSampler();
	}
	if (anisotropicSamplerState == nullptr) {
		InitAnisoSampler();
	}
	if (dsState == nullptr) {
		InitDSState();
	}
	if (dsView == nullptr) {
		InitDSView();
	}
	if (dsBuffer == nullptr) {
		InitDSBuffer();
	}
	*/
}

IDXGISwapChain * GraphicsInterface::GetSwapChain()
{
	return swapchain;
}

ID3D11Device * GraphicsInterface::GetDevice()
{
	return device;
}

ID3D11DeviceContext * GraphicsInterface::GetDeviceContext()
{
	return devcon;
}

ID3D11RasterizerState * GraphicsInterface::GetRasterizerState()
{
	if (!rasterizerState) {
		InitRasterizerState();
	}
	return rasterizerState;
}

ID3D11RasterizerState * GraphicsInterface::GetWireframeState()
{
	if (!wireframeRasterizerState) {
		InitWireframeState();
	}
	return wireframeRasterizerState;
}

ID3D11SamplerState * GraphicsInterface::GetLinearSampler()
{
	if (!linearSamplerState) {
		InitLinearSampler();
	}
	return linearSamplerState;
}

ID3D11SamplerState * GraphicsInterface::GetAnisoSampler()
{
	if (!anisotropicSamplerState) {
		InitAnisoSampler();
	}
	return anisotropicSamplerState;
}

ID3D11RenderTargetView * GraphicsInterface::GetRenderTargetView()
{
	if (rtv == nullptr) {
		InitRTV();
	}
	return rtv;
}

D3D11_VIEWPORT * GraphicsInterface::GetViewPort()
{
	InitViewPort();
	return &viewport;
}

ID3D11VertexShader * GraphicsInterface::GetBaseVertexShader()
{
	if (baseVertexShader == nullptr) {
		InitBaseVertexShader();
	}
	return baseVertexShader;
}

ID3D11InputLayout * GraphicsInterface::GetBaseVSInputLayout()
{
	if (baseVSInputLayout == nullptr) {
		InitBaseVertexShader();
	}
	return baseVSInputLayout;
}

ID3D11PixelShader * GraphicsInterface::GetBasePixelShader()
{
	if (basePixelShader == nullptr) {
		InitBasePixelShader();
	}
	return basePixelShader;
}

ID3D11Buffer * GraphicsInterface::GetBaseCubeVertexBuffer()
{
	if (baseCubeBuffer == nullptr) {
		InitBaseCubeVertexBuffer();
	}
	return baseCubeBuffer;
}

ID3D11Buffer * GraphicsInterface::GetBaseCubeIndexBuffer()
{
	if (baseCubeIndexBuffer == nullptr) {
		InitBaseCubeVertexBuffer();
	}
	return baseCubeIndexBuffer;
}

int GraphicsInterface::GetBaseCubeIndicesCount()
{
	return baseCubeIndexCount;
}

void GraphicsInterface::CleanD3D(void)
{
	//close and release all existing COM objects
	swapchain->Release();
	device->Release();
	devcon->Release();

	rasterizerState->Release();
	wireframeRasterizerState->Release();
	linearSamplerState->Release();
	anisotropicSamplerState->Release();

	dsState->Release();
	dsView->Release();
	dsBuffer->Release();

	rtv->Release();
	baseVertexShader->Release();
	baseVSInputLayout->Release();
	basePixelShader->Release();

	baseCubeBuffer->Release();
	baseCubeIndexBuffer->Release();
}

GraphicsInterface * GraphicsInterface::Get()
{
	if (!instance)
		instance = new GraphicsInterface();
	return instance;
}
