#include "WindowContainer.h"
//#include "imgui_impl_win32.h"

WindowContainer::WindowContainer()
{
	//this->mouse = new Mouse();
	//this->keyboard = new Keyboard();

	static bool raw_input_initialized = false;
	if (raw_input_initialized == false)
	{
		RAWINPUTDEVICE rid;

		rid.usUsagePage = 0x01; //Mouse
		rid.usUsage = 0x02;
		rid.dwFlags = 0;
		rid.hwndTarget = NULL;

		if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
		{
			std::cout << "error register raw input device" << std::endl;
			exit(-1);
		}

		raw_input_initialized = true;
	}
}

WindowContainer::~WindowContainer()
{
	//delete mouse;
	//delete keyboard;
}

LRESULT WindowContainer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//if (!mouse->getMouseActive() && ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
	//{
	//	return true;
	//}
	switch (uMsg)
	{
	case WM_KEYUP:
	{
		unsigned char ch = static_cast<unsigned char>(wParam);
		//keyboard->onKeyReleased(ch);
		return 0;
	}
	case WM_KEYDOWN:
	{
		unsigned char ch = static_cast<unsigned char>(wParam);
		//keyboard->onKeyDown(ch);
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		//mouse->onMouseMove(x, y);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		//mouse->onLeftPressed(x, y);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		//mouse->onRightPressed(x, y);
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		//mouse->onMiddlePressed(x, y);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		//mouse->onLeftReleased(x, y);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		//mouse->onRightReleased(x, y);
		return 0;
	}
	case WM_MBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		//mouse->onMiddleReleased(x, y);
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			//mouse->onWheelUp(x, y);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
		{
			//mouse->onWheelDown(x, y);
		}
		return 0;
	}
	case WM_INPUT:
	{
		//if (mouse->getMouseActive()) {
		//	UINT dataSize;
		//	GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER)); //Need to populate data size first
		//
		//	if (dataSize > 0)
		//	{
		//		std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
		//		if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
		//		{
		//			RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
		//			if (raw->header.dwType == RIM_TYPEMOUSE)
		//			{
		//				//std::cout << raw->data.mouse.lLastX << ":" << raw->data.mouse.lLastY << std::endl;
		//				mouse->onMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
		//			}
		//		}
		//	}
		//}
		return DefWindowProc(hwnd, uMsg, wParam, lParam); //Need to call DefWindowProc for WM_INPUT messages
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

RenderWindow& WindowContainer::getRenderWindow()
{
	return this->render_window;
}

//Mouse*& WindowContainer::getMouse()
//{
//	return this->mouse;
//}

//Keyboard*& WindowContainer::getKeyboard()
//{
//	return this->keyboard;
//}

