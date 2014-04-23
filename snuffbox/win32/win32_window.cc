#include "../../snuffbox/win32/win32_window.h"
#include "../../snuffbox/logging.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/game.h"
#include "../../snuffbox/memory/allocated_memory.h"
#include "../../snuffbox/input/mouse.h"
#include <string>

#define WIN32_GET_MOUSE_POS POINT p;if (GetCursorPos(&p)){ScreenToClient(hWnd, &p);}

namespace snuffbox
{
	//---------------------------------------------------------------------------
	Win32Window::Win32Window() : focussed_(false)
	{
		SNUFF_LOG_INFO("Constructed the window");
	}

	//---------------------------------------------------------------------------
	Win32Window::Win32Window(const char* name, int w, int h, int x, int y) : focussed_(false)
	{
		params().x = x;
		params().y = y;
		params().w = w;
		params().h = h;
		params().name = name;


		std::string paramX = x == SNUFF_WINDOW_CENTERED ? "centered" : std::to_string(params().x);
		std::string paramY = y == SNUFF_WINDOW_CENTERED ? "centered" : std::to_string(params().y);

		std::string paramW = std::to_string(params().w);
		std::string paramH = std::to_string(params().h);

		std::string strName = params().name;
		std::string result = 
			"Constructed a window with name: " + strName +
			"\n\tParameters: " +
			"\n\tWidth: " + paramW +
			"\n\tHeight: " + paramH +
			"\n\tX: " + paramX +
			"\n\tY: " + paramY;

		SNUFF_LOG_INFO(result.c_str());
	}

	//---------------------------------------------------------------------------
	void Win32Window::Create()
	{
		WNDCLASSA wndClass;

    instance_ = GetModuleHandle(0);

		ZeroMemory(&wndClass, sizeof(WNDCLASS));

		wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wndClass.lpfnWndProc = WndProc;
    wndClass.hInstance = instance_;
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)BLACK_BRUSH;
		wndClass.lpszClassName = SNUFF_WINDOW_CLASS;
		wndClass.cbWndExtra = sizeof(void*);
		wndClass.cbClsExtra = sizeof(WNDCLASS);

		if (!RegisterClassA(&wndClass))
		{
			SNUFF_LOG_FATAL("Could not register window class!");
			return;
		}

		RECT clientSize;
		clientSize.left = clientSize.top = 0;
		clientSize.right = params().w;
		clientSize.bottom = params().h;

    int style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_MINIMIZEBOX;

		AdjustWindowRect(&clientSize, style, FALSE);
		unsigned int actualWidth = clientSize.right - clientSize.left;
		unsigned int actualHeight = clientSize.bottom - clientSize.top;

		params().x = params().x == SNUFF_WINDOW_CENTERED ? (GetSystemMetrics(SM_CXSCREEN) - actualWidth) / 2 : params().x;
		params().y = params().y == SNUFF_WINDOW_CENTERED ? (GetSystemMetrics(SM_CYSCREEN) - actualHeight) / 2 : params().y;

		auto name = params().name;

		handle_ = CreateWindowA(wndClass.lpszClassName, params().name,
      style, params().x, params().y, params().w, params().h, GetDesktopWindow(), NULL,
			wndClass.hInstance, this);
		if (!handle_)
		{
			SNUFF_LOG_FATAL("Unable to open window!");
			return;
		}

		ShowWindow(handle_, SW_SHOWMINIMIZED);
	}

	//---------------------------------------------------------------------------
	void Win32Window::Show()
	{
    ShowWindow(handle_, SW_SHOWNORMAL);
	}

	//---------------------------------------------------------------------------
	void Win32Window::Destroy()
	{
		SNUFF_ASSERT_NOTNULL(handle_);
		DestroyWindow(handle_);
    UnregisterClassA(SNUFF_WINDOW_CLASS, instance_);

		std::string name = params().name;
		std::string result = "Destroyed the window with name: " + name;
		SNUFF_LOG_INFO(result.c_str());
	}
	
	//---------------------------------------------------------------------------
	void Win32Window::OnMouseMove(float x, float y)
	{
		MouseData evt;

    evt.x = x;
    evt.y = y;
		evt.type = MouseEvent::kMove;

		environment::mouse().ReceiveEvent(evt);
	}

	//---------------------------------------------------------------------------
	void Win32Window::OnMouseDown(MouseButton button, float x, float y)
	{
		MouseData evt;
    
    evt.x = x;
    evt.y = y;
		evt.button = button;
		evt.type = MouseEvent::kPressed;

		environment::mouse().ReceiveEvent(evt);
	}

	//---------------------------------------------------------------------------
	void Win32Window::OnMouseUp(MouseButton button)
	{
		MouseData evt;
		evt.type = MouseEvent::kUp;
		evt.button = button;

		environment::mouse().ReceiveEvent(evt);
	}

	void Win32Window::OnMouseDbl(MouseButton button, float x, float y)
	{
		MouseData evt;
		evt.type = MouseEvent::kDblClk;
		evt.button = button;

		environment::mouse().ReceiveEvent(evt);
	}

	//---------------------------------------------------------------------------
	void Win32Window::OnSetFocus()
	{
		SNUFF_LOG_INFO("[WIN32] The window received focus");
		focussed_ = true;
	}

	//---------------------------------------------------------------------------
	void Win32Window::OnKillFocus()
	{
		SNUFF_LOG_INFO("[WIN32] The window lost focus");
		focussed_ = false;
	}

	//---------------------------------------------------------------------------
	void Win32Window::OnClose()
	{
		environment::game().NotifyEvent(GameEvents::kQuit);
	}

	//---------------------------------------------------------------------------
	Win32Window::~Win32Window()
	{
		
	}

	//---------------------------------------------------------------------------
	void Win32Window::ProcessMessages()
	{
		if (handle_ != nullptr)
		{
			HWND hwnd = static_cast<HWND>(handle_);
			MSG msg;
			while (PeekMessageA(&msg, hwnd, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessageA(&msg);
			}
		}
	}

	//---------------------------------------------------------------------------
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_CREATE)
		{
			SetWindowLongPtrA(hWnd, GWLP_USERDATA, (LONG_PTR)(((LPCREATESTRUCT)lParam)->lpCreateParams));
			return DefWindowProcA(hWnd, message, wParam, lParam);
		}

		Win32Window *window = reinterpret_cast<Win32Window*>(GetWindowLongPtrA(hWnd, GWLP_USERDATA));

    WIN32_GET_MOUSE_POS;

		switch (message)
		{
		case WM_SYSCOMMAND:
			if (wParam == SC_KEYMENU && (lParam >> 16) <= 0)
				return 0;
			break;

		case WM_SETFOCUS:
			window->OnSetFocus();
			break;

		case WM_KILLFOCUS:
			window->OnKillFocus();
			break;

		case WM_CLOSE:
			window->OnClose();
			break;
			
		case WM_LBUTTONDBLCLK:
      window->OnMouseDbl(MouseButton::kLeft, static_cast<float>(p.x), static_cast<float>(p.y));
			break;

		case WM_LBUTTONDOWN:
			window->OnMouseDown(MouseButton::kLeft, static_cast<float>(p.x),static_cast<float>(p.y));
			break;

		case WM_LBUTTONUP:
			window->OnMouseUp(MouseButton::kLeft);
			break;

		case WM_RBUTTONDBLCLK:
			window->OnMouseDbl(MouseButton::kRight, static_cast<float>(p.x), static_cast<float>(p.y));
			break;

		case WM_RBUTTONDOWN:
			window->OnMouseDown(MouseButton::kRight, static_cast<float>(p.x), static_cast<float>(p.y));
			break;

		case WM_RBUTTONUP:
			window->OnMouseUp(MouseButton::kRight);
			break;

		case WM_MBUTTONDBLCLK:
			window->OnMouseDbl(MouseButton::kMiddle, static_cast<float>(p.x), static_cast<float>(p.y));
			break;

		case WM_MBUTTONDOWN:
			window->OnMouseDown(MouseButton::kMiddle, static_cast<float>(p.x), static_cast<float>(p.y));
			break;

		case WM_MBUTTONUP:
			window->OnMouseUp(MouseButton::kMiddle);
			break;

		case WM_MOUSEMOVE:
      window->OnMouseMove(static_cast<float>(p.x), static_cast<float>(p.y));
			break;
		}
		return DefWindowProcA(hWnd, message, wParam, lParam);
	}
}