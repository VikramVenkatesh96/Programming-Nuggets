#include "Application.h"
#include "Window.h"

void Application::SetWindowClearColor(float r, float g, float b, float a)
{
   appWindow->SetClearColor(r, g, b, a); 
}

Application::Application(int startPX, int startPY, int width, int height, const wchar_t* appName)
{
	appWindow = new Window(startPX, startPY, width, height, appName);
	bExitApp = false;
}

Application::~Application()
{
	delete appWindow;
}

void Application::Update_Internal()
{
    while (!ShouldExitApp())
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                SetExitApp(1);
        }
        if (ShouldExitApp())
            break;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        appWindow->Update();

        ImGuiIO& io = ImGui::GetIO();

        // Handle custom user logic
        Update(io.DeltaTime);

        // Render to Window
        appWindow->Render();
    }

}
