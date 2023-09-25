#pragma once
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <d3d11.h>
#include <tchar.h>


struct D3DDevice
{
public:
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	void SetClearColor(float r, float g, float b, float a);
	void HandleResize();
	void Render();

public :
	// Data
	ID3D11Device* pd3dDevice = nullptr;
	ID3D11DeviceContext* pd3dDeviceContext = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	UINT                     resizeWidth = 0, resizeHeight = 0;
	ID3D11RenderTargetView* mainRenderTargetView = nullptr;
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};

class Window
{
private:
	class WindowClass
	{
	public:
		static LPCWSTR GetName();
		static HINSTANCE GetInstance();
	
	private:
		WindowClass();
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
	private:
		static constexpr LPCWSTR wcName = L" Programming Nuggets";
		HINSTANCE hInst;
		static WindowClass wc;
	};
public:
	void Update();
	void Render();
	inline void SetClearColor(float r, float g, float b, float a) { dxDevice.SetClearColor(r, g, b, a); }
public:
	Window(int startPX, int startPY, int width, int height, LPCWSTR winName);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
private:
	static LRESULT WINAPI HandleMessagesSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI HandleMessagesMain(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
	LRESULT WINAPI HandleMessages(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	D3DDevice dxDevice;
	int width, height;
	HWND hWnd;
};

