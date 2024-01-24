#include <iostream>
#include "WindowClass.h"
#include "Debugger.h"
#include "Graphics.h"

#include "TestObject.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//enable console
#ifdef _DEBUG
	RedirectIOToConsole();
#endif
	
	MSG msg = {};

	getGFX.init();

	TestObject test;
	float x = 0;
	while (msg.message != WM_QUIT && getGFX.processMessages())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		getGFX.changeBackgroundColor(sin(x), 0, 0);
		x += 0.01f;
		getGFX.beginFrame();
		test.render();
		getGFX.endFrame();
	}

	return 0;
}