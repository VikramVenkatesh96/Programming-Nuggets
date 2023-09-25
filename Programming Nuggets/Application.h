#pragma once

// Frwd declaring
class Window;

class Application
{
public:
	virtual void Start() = 0;
	virtual void Update(float deltaTime) = 0;
	void SetWindowClearColor(float r, float g, float b, float a);
public:
	Application(int startPX, int startPY, int width, int height, const wchar_t* appName);
	virtual ~Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
public:
	void Update_Internal();
	inline bool ShouldExitApp() { return bExitApp; }
	inline void SetExitApp(bool bExit) { bExitApp = bExit; }
private:
	Window* appWindow;
	bool bExitApp;
};

