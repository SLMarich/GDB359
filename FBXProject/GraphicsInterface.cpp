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
	viewportInitialized = false;

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

	teddyIndexCount = 0;
	teddyMeshBuffer = nullptr;
	teddyMeshIndexBuffer = nullptr;
	teddyPixelShader = nullptr;
	teddyVertexShader = nullptr;
	teddyVSInputLayout = nullptr;
	
	debugGroundBuffer = nullptr;
	debugGroundInputLayout = nullptr;
	debugGroundPixelShader = nullptr;
	debugGroundVertexCount = 0;
	debugGroundVertexShader = nullptr;
}

void GraphicsInterface::InitRasterizerState()
{
	//Set up Rasterizer
	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	//rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
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
	//depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

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

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	//depthStencilViewDesc.Flags = 0;
	
	if (dsBuffer == nullptr) {
		InitDSBuffer();
	}

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
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
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
	viewportInitialized = true;
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
		{"COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
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

void GraphicsInterface::InitTeddyShaders()
{
	std::vector<byte> csoFile = csoReader::readCSOFile("TeddyVertexShader.cso");
	HRESULT result = device->CreateVertexShader(&csoFile[0], csoFile.size(),NULL,&teddyVertexShader);
	//std::cout << "\nTeddy Vertex Shader initialized. Pointer to: " << teddyVertexShader << std::endl;

	D3D11_INPUT_ELEMENT_DESC teddyVertexDesc[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	//std::cout << "\nTeddy Vertex Shader Input Layout: " << std::endl;
	//std::cout << "Array Size: " << ARRAYSIZE(teddyVertexDesc) << std::endl;
	//std::cout << "csoFile location: " << (void*)&csoFile[0] << std::endl;
	//std::cout << "Before CreateInputLayout call, vsIL: " << teddyVSInputLayout << std::endl;
	result = device->CreateInputLayout(teddyVertexDesc, ARRAYSIZE(teddyVertexDesc), &csoFile[0], csoFile.size(), &teddyVSInputLayout);
	//std::cout << "Teddy Vertex Shader input layout initialized. Pointer to: " << teddyVSInputLayout << std::endl;

	csoFile = csoReader::readCSOFile("TeddyPixelShader.cso");
	device->CreatePixelShader(&csoFile[0], csoFile.size(), NULL, &teddyPixelShader);
}

void GraphicsInterface::InitCameraMatrix()
{
	float aspectRatio = viewport.Width/viewport.Height;
	float fovAngleY = 70.0f * DirectX::XM_PI / 180.0f;
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}
	DirectX::XMMATRIX perspectiveMatrix = DirectX::XMMatrixPerspectiveFovRH(fovAngleY, fovAngleY, nearPlane, farPlane);

	DirectX::XMMATRIX orientationMatrix = DirectX::XMMatrixIdentity();

	XMStoreFloat4x4(&projection, DirectX::XMMatrixTranspose(perspectiveMatrix * orientationMatrix));
	projectionMatrix = DirectX::XMMatrixTranspose(perspectiveMatrix * orientationMatrix);

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	static const DirectX::XMVECTORF32 eye = { 0.0f, 1.5f, -10.0f, 0.0f };
	static const DirectX::XMVECTORF32 at = { 0.0f, 1.0f, 0.0f, 0.0f };
	static const DirectX::XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	cameraMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtRH(eye, at, up));
	XMStoreFloat4x4(&camera, DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtRH(eye, at, up)));
}

void GraphicsInterface::InitModelConstantBuffer()
{
	DirectX::XMFLOAT4X4 identity4x4 = { 1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f };
	static const ModelViewProjection mvpData[] = {
		{identity4x4, camera, projection}
	};
	D3D11_SUBRESOURCE_DATA mvpSubresource;
	//ZeroMemory(&mvpSubresource, sizeof(mvpSubresource));
	mvpSubresource.pSysMem = mvpData;
	mvpSubresource.SysMemPitch = 0;
	mvpSubresource.SysMemSlicePitch = 0;

	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjection), D3D11_BIND_CONSTANT_BUFFER);

	
	device->CreateBuffer(&constantBufferDesc, &mvpSubresource, &modelConstantBuffer);
}

void GraphicsInterface::InitDebugGround()
{
	std::vector<byte> csoFile = csoReader::readCSOFile("DebugGroundVertexShader.cso");
	HRESULT result = device->CreateVertexShader(&csoFile[0], csoFile.size(), NULL, &debugGroundVertexShader);

	D3D11_INPUT_ELEMENT_DESC debugGroundVertexDesc[] = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	result = device->CreateInputLayout(debugGroundVertexDesc, ARRAYSIZE(debugGroundVertexDesc), &csoFile[0], csoFile.size(), &debugGroundInputLayout);

	csoFile = csoReader::readCSOFile("DebugGroundPixelShader.cso");
	device->CreatePixelShader(&csoFile[0], csoFile.size(), NULL, &debugGroundPixelShader);

	std::vector<DirectX::XMFLOAT3> debugGroundVertices;
	DirectX::XMFLOAT3 holder;
	for (int i = 0; i < debugGroundWidth; i++) {
		for (int j = 0; j < debugGroundHeight; j++) {
			//Start point for x
			holder.x = j - (debugGroundWidth / 2.0f);
			holder.y = 0.0f;
			holder.z = (debugGroundHeight / 2.0f);
			debugGroundVertices.push_back(holder);
			//End point for x
			holder.x = j - (debugGroundWidth / 2.0f);
			holder.y = 0.0f;
			holder.z = -(debugGroundHeight / 2.0f);
			debugGroundVertices.push_back(holder);
			//Start point for z
			holder.x = (debugGroundWidth / 2.0f);
			holder.y = 0.0f;
			holder.z = i - (debugGroundHeight / 2.0f);
			debugGroundVertices.push_back(holder);
			//End point for z
			holder.x = -(debugGroundWidth / 2.0f);
			holder.y = 0.0f;
			holder.z = i - (debugGroundHeight / 2.0f);
			debugGroundVertices.push_back(holder);
		}
	}
	debugGroundVertexCount = debugGroundVertices.size();

	D3D11_SUBRESOURCE_DATA debugGroundVertexBufferData = { 0 };
	debugGroundVertexBufferData.pSysMem = debugGroundVertices.data();
	debugGroundVertexBufferData.SysMemPitch = 0;
	debugGroundVertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC debugGroundVertexBufferDesc(debugGroundVertices.size()*sizeof(DirectX::XMFLOAT3), D3D11_BIND_VERTEX_BUFFER);
	device->CreateBuffer(
		&debugGroundVertexBufferDesc,
		&debugGroundVertexBufferData,
		&debugGroundBuffer
	);
}

void GraphicsInterface::InitJointShaders()
{
	std::vector<byte> csoFile = csoReader::readCSOFile("JointVertexShader.cso");
	HRESULT result = device->CreateVertexShader(&csoFile[0], csoFile.size(), NULL, &jointVertexShader);

	D3D11_INPUT_ELEMENT_DESC jointVertexDesc[] = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "POSITION",1,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "POSITION",2,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "POSITION",3,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	result = device->CreateInputLayout(jointVertexDesc, ARRAYSIZE(jointVertexDesc), &csoFile[0], csoFile.size(), &jointInputLayout);

	csoFile = csoReader::readCSOFile("JointPixelShader.cso");
	device->CreatePixelShader(&csoFile[0], csoFile.size(), NULL, &jointPixelShader);
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
	scd.SampleDesc.Count = 1;							//how many multisamples
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
	*/
	if (dsState == nullptr) {
		InitDSState();
	}
	if (dsView == nullptr) {
		InitDSView();
	}
	if (dsBuffer == nullptr) {
		InitDSBuffer();
	}
	InitCameraMatrix();
	InitModelConstantBuffer();
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
	if (viewportInitialized == false) {
		InitViewPort();
	}
	return &viewport;
}

ID3D11DepthStencilView * GraphicsInterface::GetDepthStencilView()
{
	if (dsView == nullptr) {
		InitDSView();
	}
	return dsView;
}

ID3D11Texture2D * GraphicsInterface::GetDepthBuffer()
{
	if (dsBuffer == nullptr) {
		InitDSBuffer();
	}
	return dsBuffer;
}

ID3D11DepthStencilState * GraphicsInterface::GetDepthStencilState()
{
	if (dsState == nullptr) {
		InitDSState();
	}
	return dsState;
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

DirectX::XMFLOAT4X4 GraphicsInterface::GetCameraMatrix()
{
	return camera;
}

DirectX::XMFLOAT4X4 GraphicsInterface::GetProjectionMatrix()
{
	return projection;
}

ID3D11Buffer * GraphicsInterface::GetModelConstantBuffer()
{
	return modelConstantBuffer;
}

void GraphicsInterface::UpdateModelConstantBuffer(ModelViewProjection newMvp)
{
	devcon->UpdateSubresource(modelConstantBuffer, 0, NULL, &newMvp, 0, 0);
}

float GraphicsInterface::GetFarPlane()
{
	return farPlane;
}

float GraphicsInterface::GetNearPlane()
{
	return nearPlane;
}

void GraphicsInterface::MakeTeddyBuffers(std::vector<DirectX::XMFLOAT4> verts, std::vector<int> inds)
{
	D3D11_SUBRESOURCE_DATA teddyVertexBufferData = { 0 };
	teddyVertexBufferData.pSysMem = verts.data();
	teddyVertexBufferData.SysMemPitch = 0;
	teddyVertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC teddyVertexBufferDesc(verts.size()*sizeof(DirectX::XMFLOAT4), D3D11_BIND_VERTEX_BUFFER);
	device->CreateBuffer(
		&teddyVertexBufferDesc,
		&teddyVertexBufferData,
		&teddyMeshBuffer
	);

	D3D11_SUBRESOURCE_DATA teddyIndexBufferData = { 0 };
	teddyIndexBufferData.pSysMem = inds.data();
	teddyIndexBufferData.SysMemPitch = 0;
	teddyIndexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC teddyIndexBufferDesc(inds.size()*sizeof(int), D3D11_BIND_INDEX_BUFFER);
	device->CreateBuffer(
		&teddyIndexBufferDesc,
		&teddyIndexBufferData,
		&teddyMeshIndexBuffer
	);

}

ID3D11VertexShader * GraphicsInterface::GetTeddyVertexShader()
{
	if (teddyVertexShader == nullptr) {
		InitTeddyShaders();
	}
	return teddyVertexShader;
}

ID3D11InputLayout * GraphicsInterface::GetTeddyVSInputLayout()
{
	if (teddyVSInputLayout == nullptr) {
		InitTeddyShaders();
	}
	return teddyVSInputLayout;
}

ID3D11PixelShader * GraphicsInterface::GetTeddyPixelShader()
{
	if (teddyPixelShader == nullptr) {
		InitTeddyShaders();
	}
	return teddyPixelShader;
}

ID3D11Buffer * GraphicsInterface::GetTeddyVertexBuffer()
{
	return teddyMeshBuffer;
}

ID3D11Buffer * GraphicsInterface::GetTeddyIndexBuffer()
{
	return teddyMeshIndexBuffer;
}

ID3D11Buffer * GraphicsInterface::GetDebugGroundVertexBuffer()
{
	if (debugGroundBuffer == nullptr) {
		InitDebugGround();
	}
	return debugGroundBuffer;
}

ID3D11VertexShader * GraphicsInterface::GetDebugGroundVertexShader()
{
	if (debugGroundVertexShader == nullptr) {
		InitDebugGround();
	}
	return debugGroundVertexShader;
}

ID3D11InputLayout * GraphicsInterface::GetDebugGroundInputLayout()
{
	if (debugGroundInputLayout == nullptr) {
		InitDebugGround();
	}
	return debugGroundInputLayout;
}

ID3D11PixelShader * GraphicsInterface::GetDebugGroundPixelShader()
{
	if (debugGroundPixelShader == nullptr) {
		InitDebugGround();
	}
	return debugGroundPixelShader;
}

int GraphicsInterface::GetDebugGroundVertexCount()
{
	if (debugGroundVertexCount == 0) {
		InitDebugGround();
	}
	return debugGroundVertexCount;
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

	teddyMeshBuffer->Release();
	teddyMeshIndexBuffer->Release();
	teddyPixelShader->Release();
	teddyVertexShader->Release();
	teddyVSInputLayout->Release();

	debugGroundBuffer->Release();
	debugGroundInputLayout->Release();
	debugGroundPixelShader->Release();
	debugGroundVertexShader->Release();

}

GraphicsInterface * GraphicsInterface::Get()
{
	if (!instance)
		instance = new GraphicsInterface();
	return instance;
}
