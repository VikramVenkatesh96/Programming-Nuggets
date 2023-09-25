#include "Window.h"

Window::WindowClass Window::WindowClass::wc;

LPCWSTR Window::WindowClass::GetName()
{
    return wcName;
}

HINSTANCE Window::WindowClass::GetInstance()
{
    return wc.hInst;
}

Window::WindowClass::WindowClass():
    hInst(GetModuleHandle(nullptr))
{
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, HandleMessagesSetup , 0L, 0L, GetInstance(), nullptr, nullptr, nullptr, nullptr, GetName(), nullptr };
    ::RegisterClassExW(&wc);
}

Window::WindowClass::~WindowClass()
{
    ::UnregisterClassW(wc.GetName(), wc.GetInstance());
}


Window::Window(int startPX, int startPY, int width, int height, LPCWSTR winName):
    width(width), height(height)
{
    hWnd = ::CreateWindowW(WindowClass::GetName(), winName, WS_OVERLAPPEDWINDOW, startPX, startPY, width, height, nullptr, nullptr, WindowClass::GetInstance(), this);
    
    // Initialize Direct3D
    if (!dxDevice.CreateDeviceD3D(hWnd))
    {
        dxDevice.CleanupDeviceD3D();
        ::UnregisterClassW(WindowClass::GetName(), WindowClass::GetInstance());
        exit(1);
    }

    // Show the window
    ::ShowWindow(hWnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hWnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(dxDevice.pd3dDevice, dxDevice.pd3dDeviceContext);
}

Window::~Window()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    dxDevice.CleanupDeviceD3D();
    ::DestroyWindow(hWnd);
}


void Window::Update()
{
    dxDevice.HandleResize();

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Window::Render()
{
    ImGui::Render();
    dxDevice.Render();
}

LRESULT WINAPI Window::HandleMessagesSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_NCCREATE)
    {
        const CREATESTRUCTW* const pCreateStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWnd = static_cast<Window*>(pCreateStruct->lpCreateParams);
        // Set this window's userdata to point to this instance of window
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        // After setup is done, we use the HandleMessagesMain 
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMessagesMain));

        return pWnd->HandleMessages(hWnd, msg, wParam, lParam);
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMessagesMain(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Retrieve the stored window info from setup
    Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    // Now we can use our handlemessages function from each instance
    return pWnd->HandleMessages(hWnd, msg, wParam, lParam);
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI Window::HandleMessages(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        dxDevice.resizeWidth = (UINT)LOWORD(lParam); // Queue resize
        dxDevice.resizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

bool D3DDevice::CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, &featureLevel, &pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, &featureLevel, &pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void D3DDevice::CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (pSwapChain) { pSwapChain->Release(); pSwapChain = nullptr; }
    if (pd3dDeviceContext) { pd3dDeviceContext->Release(); pd3dDeviceContext = nullptr; }
    if (pd3dDevice) { pd3dDevice->Release(); pd3dDevice = nullptr; }
}


void D3DDevice::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
    pBackBuffer->Release();
}

void  D3DDevice::CleanupRenderTarget()
{
    if (mainRenderTargetView) { mainRenderTargetView->Release(); mainRenderTargetView = nullptr; }
}

void D3DDevice::SetClearColor(float r, float g, float b, float a)
{
    clearColor[0] = r; clearColor[1] = g; clearColor[2] = b; clearColor[3] = a;
}

void D3DDevice::HandleResize()
{
    // Handle window resize (we don't resize directly in the WM_SIZE handler)
    if (resizeWidth != 0 && resizeHeight != 0)
    {
        CleanupRenderTarget();
        pSwapChain->ResizeBuffers(0, resizeWidth, resizeHeight, DXGI_FORMAT_UNKNOWN, 0);
        resizeWidth = resizeHeight = 0;
        CreateRenderTarget();
    }
}

void D3DDevice::Render()
{
    pd3dDeviceContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
    pd3dDeviceContext->ClearRenderTargetView(mainRenderTargetView, clearColor);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    pSwapChain->Present(1, 0); // Present with vsync
    //g_pSwapChain->Present(0, 0); // Present without vsync
}
