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

	//std::string boneFilePath = "Assets\\Test Assets\\Bone.fbx";
	//FBXLibrary::FBXInterface fbxInterface;
	//fbxInterface.InitializeImporter(boneFilePath.c_str());
	//fbxInterface.CreateScene();


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
	std::cout << "Initializing Graphics Interface." << std::endl;
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
	gInt->GetDeviceContext()->ClearRenderTargetView(gInt->GetRenderTargetView(), gray);

#ifdef _DEBUG
	std::cout << "Cleared main window to gray." << std::endl;
#endif
#pragma endregion



    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FBXPROJECT));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
			//std::cout<<"This is inside the message loop."<<std::endl;
			//Set Viewport
			gInt->GetDeviceContext()->RSSetViewports(1, gInt->GetViewPort());
			gInt->GetDeviceContext()->RSSetState(gInt->GetRasterizerState());

			ID3D11Buffer* vertexBuffers[] = { gInt->GetBaseCubeVertexBuffer() };
			//gInt->GetDeviceContext()->VSSetConstantBuffers(0, 1, vertexBuffers);
			UINT stride = sizeof(BasePosition);
			UINT offset = 0;
			ID3D11Buffer* indexBuffers[] = { gInt->GetBaseCubeIndexBuffer() };
			gInt->GetDeviceContext()->IASetInputLayout(gInt->GetBaseVSInputLayout());
			gInt->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			gInt->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffers,&stride,&offset);
			gInt->GetDeviceContext()->VSSetShader(gInt->GetBaseVertexShader(), nullptr, 0);
			gInt->GetDeviceContext()->PSSetShader(gInt->GetBasePixelShader(), nullptr, 0);
			gInt->GetDeviceContext()->IASetIndexBuffer(gInt->GetBaseCubeIndexBuffer(), DXGI_FORMAT::DXGI_FORMAT_R16_UINT, 0);

			//gInt->GetDeviceContext()->VSSetShaderResources;

			ID3D11SamplerState* pixelSamplers[] = { gInt->GetLinearSampler() };
			gInt->GetDeviceContext()->PSSetSamplers(0, 1, pixelSamplers);
			gInt->GetDeviceContext()->DrawIndexed(gInt->GetBaseCubeIndicesCount(), 0, 0);

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
