#pragma once
#include "stdafx.h"
#include <d3d11.h>
#include "ProjectDataTypes.h"
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
	bool					viewportInitialized;

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
	ModelViewProjection		baseCubeMVP;
	int						baseCubeIndexCount = 0;
	float					nearPlane = 0.01f, farPlane = 100.0f;
	
	ID3D11Buffer			*modelConstantBuffer;

	DirectX::XMMATRIX		cameraMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX		projectionMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMFLOAT4X4		camera;
	DirectX::XMFLOAT4X4		projection;

	ID3D11Buffer			*teddyMeshBuffer;
	ID3D11Buffer			*teddyMeshIndexBuffer;
	int						teddyIndexCount;
	ID3D11VertexShader		*teddyVertexShader;
	ID3D11InputLayout		*teddyVSInputLayout;
	ID3D11PixelShader		*teddyPixelShader;

	ID3D11Buffer			*jointVertexBuffer;
	ID3D11Buffer			*jointInstanceBuffer;
	ID3D11VertexShader		*jointVertexShader;
	ID3D11InputLayout		*jointInputLayout;
	ID3D11PixelShader		*jointPixelShader;
	
	ID3D11Buffer			*debugGroundBuffer;
	ID3D11VertexShader		*debugGroundVertexShader;
	ID3D11InputLayout		*debugGroundInputLayout;
	ID3D11PixelShader		*debugGroundPixelShader;
	int						debugGroundVertexCount;
	int debugGroundWidth = 100, debugGroundHeight = 100;

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

	void InitTeddyShaders();	//Initializes the teddy shaders

	void InitCameraMatrix();
	void InitModelConstantBuffer(); //Initializes the model constant buffer

	void InitDebugGround();
	void InitJointShaders();

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
	ID3D11DepthStencilView* GetDepthStencilView();	//Returns the pointer to the depth stencil view
	ID3D11Texture2D* GetDepthBuffer();				//Returns the pointer to the depth buffer
	ID3D11DepthStencilState* GetDepthStencilState(); //Returns the pointer to the depth stencil state
	ID3D11VertexShader* GetBaseVertexShader();	//Returns the base vertex shader
	ID3D11InputLayout*	GetBaseVSInputLayout();	//Returns the base vertex shader's input layout
	ID3D11PixelShader* GetBasePixelShader();	//Returns the base pixel shader
	ID3D11Buffer*	GetBaseCubeVertexBuffer();	//Returns the base cube vertex buffer
	ID3D11Buffer*	GetBaseCubeIndexBuffer();	//Returns the base cube index buffer
	int GetBaseCubeIndicesCount();				//Returns the number of indices in the index buffer

	DirectX::XMFLOAT4X4 GetCameraMatrix();		//Returns camera matrix
	DirectX::XMFLOAT4X4 GetProjectionMatrix();	//Returns projection matrix

	ID3D11Buffer* GetModelConstantBuffer();		//Returns the model constant buffer
	void UpdateModelConstantBuffer(ModelViewProjection newMvp);			//Updates the model constant buffer's subresource
	float GetFarPlane();	//returns far plane
	float GetNearPlane();	//returns near plane

	//Teddy
	void MakeTeddyBuffers(std::vector<DirectX::XMFLOAT4> verts, std::vector<int> inds);
	ID3D11VertexShader* GetTeddyVertexShader();	//Returns the teddy vertex shader
	ID3D11InputLayout* GetTeddyVSInputLayout();	//Returns the teddy vertex shader's input layout
	ID3D11PixelShader* GetTeddyPixelShader();	//Returns the teddy pixel shader
	ID3D11Buffer* GetTeddyVertexBuffer();	//Returns the teddy vertex buffer
	ID3D11Buffer* GetTeddyIndexBuffer();	//Returns the teddy index buffer

	//Joints


	//Debug Ground
	ID3D11Buffer* GetDebugGroundVertexBuffer();	//Returns the debug ground vertex buffer
	ID3D11VertexShader* GetDebugGroundVertexShader();	//Returns the debug ground vertex shader
	ID3D11InputLayout* GetDebugGroundInputLayout();	//Returns the debug ground input layout
	ID3D11PixelShader* GetDebugGroundPixelShader();	//Returns the debug ground pixel shader
	int GetDebugGroundVertexCount();	//Returns the number of vertexes for debug ground

	//Cleanup:
	void CleanD3D(void);			//closes Direct3D and releases memory

	//Singleton reference return:
	static GraphicsInterface * Get();		//Return instance

};