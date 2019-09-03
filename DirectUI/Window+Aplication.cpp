#include "Window.h"
#include "Application.h"
#include "Message.h"
#include "DrawMessage.h"
#include "Graphics.h"

#define NOMINMAX
#include <windows.h> // for Win32 API

#ifdef _MSC_VER
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

namespace directui
{

class Window::Impl
{
private:
	HWND _hwnd{ nullptr };
	bool _willDestroyPostQuit{ false };
	MessageHandler _messageHandler{ nullptr };
	std::unique_ptr<graphics::IDeviceContext> _deviceContext;

	static const wchar_t* ClassName() { return  L"DirectUIWindow"; }

	static DWORD WindowStyleFromType( WindowType type )
	{
		switch ( type )
		{
			case WindowType::Main: return WS_OVERLAPPEDWINDOW;
			case WindowType::Popup: return WS_POPUPWINDOW;
			case WindowType::Child: return WS_CHILDWINDOW;
			default: return 0;
		}
	}

public:
	Impl( WindowType type, graphics::IDevice& device, const String& name, const Rect& rc, Window* parentWindow, MessageHandler messageHandler )
	{
		RegisterOnce();

		DWORD style = WindowStyleFromType( type ) | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		HWND parentHwnd = nullptr;

		if ( parentWindow && parentWindow->_impl )
			parentHwnd = parentWindow->_impl->_hwnd;

		_messageHandler = messageHandler;
		_hwnd = ::CreateWindowExW( 0U, ClassName(), name.c_str(), style,
			rc.x, rc.y, rc.w, rc.h,
			parentHwnd, nullptr, ProgramInstance(), this );
		
		_deviceContext = device.CreateDeviceContext();
	}

	~Impl()
	{
		::DestroyWindow( _hwnd );
	}

	void Show()
	{
		::ShowWindow( _hwnd, SW_SHOW );
	}

	int MessageLoop()
	{
		_willDestroyPostQuit = true;

		MSG msg;
		while ( ::GetMessageW( &msg, nullptr, 0, 0 ) )
		{
			::TranslateMessage( &msg );
			::DispatchMessageW( &msg );
		}

		return static_cast< int >( msg.wParam );
	}

private:
	LRESULT OnMessage( UINT message, WPARAM wParam, LPARAM lParam )
	{
		if ( message == WM_DESTROY && _willDestroyPostQuit )
		{
			::PostQuitMessage( 0 );
			return FALSE;
		}

		switch ( message )
		{
			case WM_ERASEBKGND:
			{
				return TRUE;
			} break;
			case WM_PAINT:
			{
				_deviceContext->BeginDraw( _hwnd );

				if ( _messageHandler )
				{
					_messageHandler( DrawMessage{ *_deviceContext } );
				}

				_deviceContext->EndDraw();

				return FALSE;
			} break;
			default:
				break;
		}

		return ::DefWindowProcW( _hwnd, message, wParam, lParam );
	}

	static LRESULT CALLBACK WindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		Window::Impl* pThis = nullptr;

		if ( message == WM_NCCREATE )
		{
			CREATESTRUCT* pCreate = reinterpret_cast< CREATESTRUCT* >( lParam );
			pThis = reinterpret_cast< Window::Impl* >( pCreate->lpCreateParams );
			::SetWindowLongPtrW( hwnd, GWLP_USERDATA, reinterpret_cast< LONG_PTR >( pThis ) );

			pThis->_hwnd = hwnd;
		}
		else
		{
			pThis = reinterpret_cast< Window::Impl* >( ::GetWindowLongPtrW( hwnd, GWLP_USERDATA ) );
		}

		if ( pThis )
			return pThis->OnMessage( message, wParam, lParam );

		return ::DefWindowProcW( hwnd, message, wParam, lParam );
	}

	static HINSTANCE ProgramInstance()
	{
		return ::GetModuleHandle( nullptr );
	}

	static void RegisterOnce()
	{
		static bool isRegistered = false;
		if ( isRegistered )
			return;

		isRegistered = true;

		WNDCLASSEXW wc;

		wc.cbSize = sizeof( WNDCLASSEX );

		wc.style = CS_DBLCLKS | CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = ProgramInstance();
		wc.hIcon = nullptr;
		wc.hCursor = ::LoadCursorW( nullptr, ( LPCWSTR ) IDC_ARROW );
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = ClassName();
		wc.hIconSm = nullptr;

		::RegisterClassExW( &wc );
	}
};

Window::Window( WindowType type, graphics::IDevice& device, const String& name, const Rect& rc, Window* parentWindow, MessageHandler messageHandler )
{
	_impl.reset( new Impl{ type, device, name, rc, parentWindow, messageHandler } );
}

Window::~Window()
{
	_impl.reset();
}

void Window::Show()
{
	_impl->Show();
}

class Application::Impl
{
};

Application::Application()
{
	_impl.reset( new Impl{} );
	Instance() = this;
}

Application::~Application()
{
	_impl.reset();
	Instance() = nullptr;
}

Application*& Application::Instance()
{
	static Application* _instance{ nullptr };
	return _instance;
}

int Application::Run( Window& window )
{
	return window._impl->MessageLoop();
}

} // namespace directui

