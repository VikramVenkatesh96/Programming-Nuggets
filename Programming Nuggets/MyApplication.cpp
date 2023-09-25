#include "MyApplication.h"
#include "imgui.h"
#include <math.h>
#include <iostream>

void MyApplication::Start()
{
}

void MyApplication::Update(float deltaTime)
{
	static float myTime = 0.0f;

	SetWindowClearColor(sinf(myTime), 0.0f, 0.0f, 0.0f);
	myTime += deltaTime;
}
