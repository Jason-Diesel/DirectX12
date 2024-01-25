#include <iostream>
#include "TestObject.h"
#include "WindowClass.h"
#include "Debugger.h"
#include "DeltaTime.h"



int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//enable console
//#ifdef _DEBUG
	RedirectIOToConsole();
//#endif
	
	MSG msg = {};

	getGFX.init();
	DeltaTime dt;

	TestObject test;
	test.scale = DirectX::XMFLOAT3(0.2, 0.2, 0.2);
	
	float x = 0;
	while (msg.message != WM_QUIT && getGFX.processMessages())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//getGFX.changeBackgroundColor(sin(x), 0, 0);
		//x += 0.001;

		if (getGFX.getKeyboard().onceisKeyReleased(VK_TAB))
		{
			getGFX.getMouse().activateMouse(!getGFX.getMouse().getMouseActive());
		}

		getGFX.updateCamera(dt.dt());

		getGFX.beginFrame();
		test.render();
		getGFX.endFrame();
		dt.restartClock();
	}

	return 0;
}