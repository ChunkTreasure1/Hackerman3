#pragma once

#include "Volt/Core/Application.h"

extern Volt::Application* Volt::CreateApplication();

namespace Volt
{
	int Main()
	{
		Application* app = Volt::CreateApplication();
		app->Run();

		delete app;
		return 0;
	}
}

#ifdef VT_DIST

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE aHInstance, HINSTANCE aPrevHInstance, PSTR aCmdLine, int aCmdShow)
{
	return Volt::Main();
}

#else

int main()
{
	return Volt::Main();
}

#endif