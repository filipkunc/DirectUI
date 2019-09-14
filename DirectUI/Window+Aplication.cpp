#include "Window.h"
#include "Application.h"
#include "Message.h"
#include "DrawMessage.h"
#include "MouseMesage.h"
#include "Graphics.h"

#include <unordered_map>

#define NOMINMAX
#include <windows.h> // for Win32 API
#include <windowsx.h> // for GET_X_LPARAM, GET_Y_LPARAM

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
	std::unique_ptr<graphics::DeviceContext> _deviceContext;

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
	Impl( WindowType type, graphics::Device& device, const String& name, const RectPx& rcPx, Window* parentWindow, MessageHandler messageHandler )
	{
		RegisterOnce();

		DWORD style = WindowStyleFromType( type ) | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		HWND parentHwnd = nullptr;

		if ( parentWindow && parentWindow->_impl )
			parentHwnd = parentWindow->_impl->_hwnd;

		_messageHandler = messageHandler;
		_hwnd = ::CreateWindowExW( 0U, ClassName(), name.c_str(), style,
			rcPx.x, rcPx.y, rcPx.w, rcPx.h,
			parentHwnd, nullptr, ProgramInstance(), this );
		
		_deviceContext = device.CreateDeviceContext();
	}

	~Impl()
	{
		::DestroyWindow( _hwnd );
	}

	Handle GetHandle() const { return _hwnd; }
	
	float GetDpi()  const
	{
		return static_cast< float >( ::GetDpiForWindow( _hwnd ) );
	}

	void Show()
	{
		::ShowWindow( _hwnd, SW_SHOW );
	}

	void Redraw( WindowRedraw redraw )
	{
		::RedrawWindow( _hwnd, nullptr, nullptr, redraw == WindowRedraw::Invalidate ? RDW_INVALIDATE : RDW_UPDATENOW );
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
			case WM_MOUSEMOVE:
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_LBUTTONDBLCLK:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_MBUTTONDBLCLK:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_RBUTTONDBLCLK:
			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:
			case WM_XBUTTONDBLCLK:
			{
				int x = GET_X_LPARAM( lParam );
				int y = GET_Y_LPARAM( lParam );

				struct MouseButtonAndState { MouseButton button; MouseState state; };

				std::unordered_map<UINT, MouseButtonAndState> mapMessageToMouse
				{
					{ WM_MOUSEMOVE,		{ MouseButton::None,	MouseState::Move		} },
					{ WM_LBUTTONDOWN,	{ MouseButton::Left,	MouseState::Down		} },
					{ WM_LBUTTONUP,		{ MouseButton::Left,	MouseState::Up			} },
					{ WM_LBUTTONDBLCLK,	{ MouseButton::Left,	MouseState::DoubleClick } },
					{ WM_MBUTTONDOWN,	{ MouseButton::Middle,	MouseState::Down		} },
					{ WM_MBUTTONUP,		{ MouseButton::Middle,	MouseState::Up			} },
					{ WM_MBUTTONDBLCLK,	{ MouseButton::Middle,	MouseState::DoubleClick } },
					{ WM_RBUTTONDOWN,	{ MouseButton::Right,	MouseState::Down		} },
					{ WM_RBUTTONUP,		{ MouseButton::Right,	MouseState::Up			} },
					{ WM_RBUTTONDBLCLK,	{ MouseButton::Right,	MouseState::DoubleClick } },
					{ WM_XBUTTONDOWN,	{ MouseButton::Other,	MouseState::Down		} },
					{ WM_XBUTTONUP,		{ MouseButton::Other,	MouseState::Up			} },
					{ WM_XBUTTONDBLCLK,	{ MouseButton::Other,	MouseState::DoubleClick } },
				};

				if ( _messageHandler )
				{
					const auto& mouse = mapMessageToMouse.at( message );
					_messageHandler( MouseMessage{ mouse.state, mouse.button, PointPx{ x, y } } );
				}
				
			} break;
			case WM_MOUSEWHEEL:
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

Window::Window( WindowType type, graphics::Device& device, const String& name, const RectPx& rcPx, Window* parentWindow, MessageHandler messageHandler )
{
	_impl.reset( new Impl{ type, device, name, rcPx, parentWindow, messageHandler } );
}

Window::~Window()
{
	_impl.reset();
}

Handle Window::GetHandle() const
{
	return _impl->GetHandle();
}

float Window::GetDpi() const
{
	return _impl->GetDpi();
}

void Window::Show()
{
	_impl->Show();
}

void Window::Redraw( WindowRedraw redraw )
{
	_impl->Redraw( redraw );
}

class Application::Impl
{
public:
	Impl()
	{
		::SetProcessDpiAwarenessContext( DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 );
	}
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

float GetSystemDpi()
{
	return static_cast< float >( ::GetDpiForSystem() );
}

} // namespace directui

