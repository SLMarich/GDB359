#pragma once
#include "stdafx.h"
#include <d3d11.h>
//include the Direct3D Library file
#pragma comment (lib,"d3d11.lib")

class GraphicsInterface {
private:
	static GraphicsInterface* instance; //Active instance of Graphics Interface

										//For Reference::
										//struct pipeline {
										//	ID3D11RasterizerState *rasterState;
										//	ID3D11RenderTargetView *render_target;
										//	ID3D11InputLayout *input_layout;
										//	ID3D11VertexShader *vertex_shader;
										//	ID3D11PixelShader * pixel_shader;
										//	ID3D11Texture2D *depthStencilBuffer;
										//	ID3D11DepthStencilState *depthStencilState;
										//	ID3D11DepthStencilView *depthStencilView;
										//};

	ID3D11Device			*device;
	ID3D11DeviceContext		*devcon;
	IDXGISwapChain			*swapchain;
	D3D_FEATURE_LEVEL		featureLevel;
	ID3D11RenderTargetView	*rtv;
	ID3D11DepthStencilView	*dsView;
	D3D11_VIEWPORT			viewport;

	ID3D11Texture2D			*dsBuffer;
	ID3D11DepthStencilState	*dsState;

	ID3D11RasterizerState	*rasterizerState;
	ID3D11RasterizerState	*wireframeRasterizerState;
	ID3D11SamplerState		*linearSamplerState;
	ID3D11SamplerState		*anisotropicSamplerState;

	ID3D11VertexShader		*baseVertexShader;
	ID3D11InputLayout		*baseVSInputLayout;
	ID3D11PixelShader		*basePixelShader;

	ID3D11Buffer			*baseCubeBuffer;
	ID3D11Buffer			*baseCubeIndexBuffer;
	int						baseCubeIndexCount = 0;


	GraphicsInterface();
	void InitRasterizerState();
	void InitWireframeState();
	void InitLinearSampler();
	void InitAnisoSampler();

	void InitDSState();
	void InitDSView();
	void InitDSBuffer();

	void InitRTV();
	void InitViewPort();
	
	void InitBaseVertexShader();	//Initializes the base vertex shader and its input layout
	void InitBasePixelShader();		//Initiates the base pixel shader

	void InitBaseCubeVertexBuffer();	//Initializes the basic cube
public:
	void InitializeD3D(HWND* hWnd);
	IDXGISwapChain* GetSwapChain();				//Returns the pointer to the swap chain
	ID3D11Device* GetDevice();					//Returns the pointer to the Direct3D device interface
	ID3D11DeviceContext* GetDeviceContext();	//Returns the pointer to our Direct3D device context
	ID3D11RasterizerState* GetRasterizerState();//Returns the pointer to the rasterizer state
	ID3D11RasterizerState* GetWireframeState();	//Returns the pointer to the wireframe rasterizer state
	ID3D11SamplerState* GetLinearSampler();		//Returns the pointer to the linear sampler state
	ID3D11SamplerState* GetAnisoSampler();		//Returns the pointer to the anisotropic sampler state
	ID3D11RenderTargetView* GetRenderTargetView();	//Returns the pointer to the render target view
	D3D11_VIEWPORT*	GetViewPort();				//Returns the view port
	ID3D11VertexShader* GetBaseVertexShader();	//Returns the base vertex shader
	ID3D11InputLayout*	GetBaseVSInputLayout();	//Returns the base vertex shader's input layout
	ID3D11PixelShader* GetBasePixelShader();	//Returns the base pixel shader
	ID3D11Buffer*	GetBaseCubeVertexBuffer();	//Returns the base cube vertex buffer
	ID3D11Buffer*	GetBaseCubeIndexBuffer();	//Returns the base cube index buffer
	int GetBaseCubeIndicesCount();				//Returns the number of indices in the index buffer

	void CleanD3D(void);			//closes Direct3D and releases memory

	static GraphicsInterface * Get();		//Return instance

};