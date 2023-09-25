#pragma once
#include "Application.h"

class MyApplication : public Application
{
public:
	void Start() override;
	void Update(float deltaTime) override;
public:
	MyApplication(int startPX, int startPY, int width, int height, const wchar_t* appName) : Application(startPX, startPY, width, height, appName) {}
};

