#include <iostream>
#include "WindowClass.h"
#include "Debugger.h"
#include "Graphics.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//enable console
#ifdef _DEBUG
	RedirectIOToConsole();
#endif
	
	Graphics gfx;
	Window window;
	window.Initialize(hInstance, "Penis", "a", 720, 480);

	while (true)
	{

	}

	return 0;
}