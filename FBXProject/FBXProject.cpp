// FBXProject.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "FBXProject.h"
#include "FBXWrapper.h"

#include "GraphicsInterface.h"
#include "ProjectDataTypes.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

#pragma region CONSOLE_ADD
#ifdef _DEBUG
	//Allocate a console for this application
	AllocConsole();
	FILE* new_std_in_out;
	freopen_s(&new_std_in_out, "CONOUT$", "w", stdout);
	freopen_s(&new_std_in_out, "CONIN$", "r", stdin);

	std::cout << "Debug mode is enabled.\n" << std::endl;
	std::cout << "Simple test function adds 5 and 4 if .dll is working: ";
	std::cout << FBXWrapper::TestFunction::add(5.0, 4.0f) << std::endl;

#endif
#ifndef _DEBUG
	FreeConsole();
#endif
#pragma endregion

#pragma region FBX_REGION
#ifdef _DEBUG
	std::cout << "Initializing FBX Interface." << std::endl;
#endif

	std::string boneFilePath = "Assets\\Test Assets\\Teddy\\Teddy_Idle.fbx";	
	FBXWrapper::FBXInterface fbxInterface;
	fbxInterface.InitializeImporter(boneFilePath.c_str());
	fbxInterface.CreateScene();
	//fbxInterface.Print();

	//Joints:
	fbxInterface.FillBindArray();
	fbxInterface.FillTransformArray();
	//fbxInterface.PrintTransformArray();
	std::vector<FBXWrapper::FBXInterface::joint> jointHolder = fbxInterface.GetTransformArray();
	
	//Mesh:
	fbxInterface.GetMesh();
	std::vector<int> teddyIndicies = fbxInterface.GetIndicies();
	std::vector<DirectX::XMFLOAT4> teddyVertices;
	DirectX::XMFLOAT4 vertexHolder;
	std::vector<FBXWrapper::FBXInterface::vector4f> verticesHolder;
	verticesHolder = fbxInterface.GetVerticies();
	for (unsigned int i = 0; i < verticesHolder.size(); i++) {
		vertexHolder.x = verticesHolder.at(i).data[0];
		vertexHolder.y = verticesHolder.at(i).data[1];
		vertexHolder.z = verticesHolder.at(i).data[2];
		vertexHolder.w = verticesHolder.at(i).data[3];
		teddyVertices.push_back(vertexHolder);
	}
	int teddyIndiciesCount = fbxInterface.GetIndiciesCount();

#ifdef _DEBUG
	std::cout << "FBX Interface created at:" << &fbxInterface << std::endl;
	//printf("Simple test of printf...\n");
#endif
#pragma endregion


    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FBXPROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

#pragma region GRAPHICS_REGION
#ifdef _DEBUG
	std::cout << "\nInitializing Graphics Interface." << std::endl;
#endif

	GraphicsInterface* gInt = GraphicsInterface::Get();
	HWND hWnd = FindWindowW(szWindowClass,NULL);
	gInt->InitializeD3D(&hWnd);
#ifdef _DEBUG
	std::cout << "hWnd: " << hWnd << std::endl;
	std::cout << "RTV: " << gInt->GetRenderTargetView() << std::endl;
#endif
	float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float gray[4] = { 0.3f,0.3f,0.3f,1.0f };
	float black[4] = { 0.0f,0.0f,0.0f,1.0f };
	gInt->GetDeviceContext()->ClearRenderTargetView(gInt->GetRenderTargetView(), black);
	bool skybox = false;
#ifdef _DEBUG
	std::cout << "Cleared main window to gray." << std::endl;
#endif
#pragma endregion



    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FBXPROJECT));

    MSG msg;

			//Set Viewport
			gInt->GetDeviceContext()->RSSetViewports(1, gInt->GetViewPort());
			ID3D11RenderTargetView*	views[] = { gInt->GetRenderTargetView() };
			gInt->GetDeviceContext()->OMSetRenderTargets(1, views, gInt->GetDepthStencilView());
			gInt->GetDeviceContext()->OMSetDepthStencilState(gInt->GetDepthStencilState(),0);
			gInt->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			float cubeRotationAngle = 0.0f;

			//Initialize the FBX information
			gInt->MakeTeddyBuffers(teddyVertices, teddyIndicies);

			// Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
			//std::cout<<"This is inside the message loop."<<std::endl;
			gInt->GetDeviceContext()->ClearRenderTargetView(gInt->GetRenderTargetView(), black);
			gInt->GetDeviceContext()->ClearDepthStencilView(gInt->GetDepthStencilView(), 0, gInt->GetFarPlane(), 0);

			cubeRotationAngle += 1.0f / 60.0f;
#pragma region SKYBOX_DRAW
			if (skybox) {
				gInt->GetDeviceContext()->RSSetState(gInt->GetRasterizerState());

				ID3D11Buffer* vertexBuffers[] = { gInt->GetBaseCubeVertexBuffer() };
				UINT stride = sizeof(BasePosition);
				UINT offset = 0;
				ID3D11Buffer* indexBuffers[] = { gInt->GetBaseCubeIndexBuffer() };
				gInt->GetDeviceContext()->IASetInputLayout(gInt->GetBaseVSInputLayout());
				gInt->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);
				gInt->GetDeviceContext()->VSSetShader(gInt->GetBaseVertexShader(), nullptr, 0);
				gInt->GetDeviceContext()->PSSetShader(gInt->GetBasePixelShader(), nullptr, 0);
				gInt->GetDeviceContext()->IASetIndexBuffer(gInt->GetBaseCubeIndexBuffer(), DXGI_FORMAT::DXGI_FORMAT_R16_UINT, 0);

				DirectX::XMFLOAT4X4 cubeLocation;
				DirectX::XMStoreFloat4x4(&cubeLocation, DirectX::XMMatrixMultiply(DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(0.0f, 0.0f, 1.0f)), DirectX::XMMatrixRotationY(cubeRotationAngle)));
				ModelViewProjection mvp;
				mvp.model = cubeLocation;
				mvp.view = gInt->GetCameraMatrix();
				mvp.projection = gInt->GetProjectionMatrix();
				gInt->UpdateModelConstantBuffer(mvp);

				ID3D11Buffer* constantBuffers[] = { gInt->GetModelConstantBuffer() };
				gInt->GetDeviceContext()->VSSetConstantBuffers(0, 1, constantBuffers);

				//gInt->GetDeviceContext()->VSSetShaderResources;

				ID3D11SamplerState* pixelSamplers[] = { gInt->GetLinearSampler() };
				gInt->GetDeviceContext()->PSSetSamplers(0, 1, pixelSamplers);
				gInt->GetDeviceContext()->DrawIndexed(gInt->GetBaseCubeIndicesCount(), 0, 0);
			}
#pragma endregion
#pragma region DEBUG_GROUND_DRAW
			gInt->GetDeviceContext()->RSSetState(gInt->GetRasterizerState());
			gInt->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

			ID3D11Buffer* debugGroundVertexBuffers[] = { gInt->GetDebugGroundVertexBuffer() };
			UINT debugGroundStride = sizeof(DirectX::XMFLOAT3);
			UINT debugGroundOffset = 0;
			gInt->GetDeviceContext()->IASetInputLayout(gInt->GetDebugGroundInputLayout());
			gInt->GetDeviceContext()->IASetVertexBuffers(0, 1, debugGroundVertexBuffers, &debugGroundStride, &debugGroundOffset);
			gInt->GetDeviceContext()->VSSetShader(gInt->GetDebugGroundVertexShader(), nullptr, 0);
			gInt->GetDeviceContext()->PSSetShader(gInt->GetDebugGroundPixelShader(), nullptr, 0);

			DirectX::XMFLOAT4X4 debugGroundLocation;
			DirectX::XMStoreFloat4x4(&debugGroundLocation,
				DirectX::XMMatrixTranspose( //Combine for model matrix and transpose for directx
					DirectX::XMMatrixTranslation(0.0f, -5.0f, 0.0f)
				));
			ModelViewProjection debuggroundmvp;
			debuggroundmvp.model = debugGroundLocation;
			debuggroundmvp.view = gInt->GetCameraMatrix();
			debuggroundmvp.projection = gInt->GetProjectionMatrix();
			gInt->UpdateModelConstantBuffer(debuggroundmvp);

			ID3D11Buffer* debugGroundConstantBuffers[] = { gInt->GetModelConstantBuffer() };
			gInt->GetDeviceContext()->VSSetConstantBuffers(0, 1, debugGroundConstantBuffers);

			ID3D11SamplerState* debugGroundPixelSamplers[] = { gInt->GetLinearSampler() };
			gInt->GetDeviceContext()->PSSetSamplers(0, 1, debugGroundPixelSamplers);
			gInt->GetDeviceContext()->Draw(gInt->GetDebugGroundVertexCount(), 0);
#pragma endregion
#pragma region TEDDY_DRAW
			gInt->GetDeviceContext()->RSSetState(gInt->GetWireframeState());
			gInt->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			ID3D11Buffer* teddyVertexBuffers[] = { gInt->GetTeddyVertexBuffer() };
			UINT teddyStride = sizeof(DirectX::XMFLOAT4);
			UINT teddyOffset = 0;
			ID3D11Buffer* teddyIndexBuffers[] = { gInt->GetTeddyIndexBuffer() };
			gInt->GetDeviceContext()->IASetInputLayout(gInt->GetTeddyVSInputLayout());
			gInt->GetDeviceContext()->IASetVertexBuffers(0, 1, teddyVertexBuffers, &teddyStride, &teddyOffset);
			gInt->GetDeviceContext()->VSSetShader(gInt->GetTeddyVertexShader(), nullptr, 0);
			gInt->GetDeviceContext()->PSSetShader(gInt->GetTeddyPixelShader(), nullptr, 0);
			gInt->GetDeviceContext()->IASetIndexBuffer(gInt->GetTeddyIndexBuffer(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

			DirectX::XMFLOAT4X4 teddyLocation;
			DirectX::XMStoreFloat4x4(&teddyLocation,
				DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply( //Combine for model matrix and transpose for directx
					DirectX::XMMatrixScaling(0.05f,0.05f,0.05f),
					DirectX::XMMatrixMultiply( //Translate and scale
						DirectX::XMMatrixRotationY(cubeRotationAngle), //Rotate
						DirectX::XMMatrixTranslation(0.0f, -5.0f, 0.0f)
					))));
			ModelViewProjection teddymvp;
			teddymvp.model = teddyLocation;
			teddymvp.view = gInt->GetCameraMatrix();
			teddymvp.projection = gInt->GetProjectionMatrix();
			gInt->UpdateModelConstantBuffer(teddymvp);

			ID3D11Buffer* teddyConstantBuffers[] = { gInt->GetModelConstantBuffer() };
			gInt->GetDeviceContext()->VSSetConstantBuffers(0, 1, teddyConstantBuffers);

			//gInt->GetDeviceContext()->VSSetShaderResources;

			ID3D11SamplerState* teddyPixelSamplers[] = { gInt->GetLinearSampler() };
			gInt->GetDeviceContext()->PSSetSamplers(0, 1, teddyPixelSamplers);
			gInt->GetDeviceContext()->DrawIndexed(teddyIndiciesCount, 0, 0);

#pragma endregion

			gInt->GetSwapChain()->Present(0, 0);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FBXPROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FBXPROJECT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
