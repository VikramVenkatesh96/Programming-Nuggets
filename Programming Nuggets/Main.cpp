#include "MyApplication.h"

int main()
{
	MyApplication myApp(0, 0, 800, 600, L"New Application");

	myApp.Start();

	myApp.Update_Internal();

	return 0;

}