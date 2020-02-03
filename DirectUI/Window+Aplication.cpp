#include "Window.h"
#include "Application.h"
#include "Graphics.h"
#include "DxGraphics.h"

#include <unordered_map>
#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h> // for Win32 API
#include <windowsx.h> // for GET_X_LPARAM, GET_Y_LPARAM
#include <dwmapi.h> // for access to Desktop Window Manager functions
#include <versionhelpers.h>

#ifdef _MSC_VER
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#pragma comment(lib, "dwmapi.lib")
#endif

namespace directui
{

class Window::Impl
{
private:
	Window& _self;

	HWND _hwnd{ nullptr };
	WindowType _type{ WindowType::Main };
	bool _activated{ false };

	bool _willDestroyPostQuit{ false };
	std::unique_ptr<graphics::DeviceContext> _deviceContext;

	static const wchar_t* ClassName() { return  L"DirectUIWindow"; }

	struct WindowStyles
	{
		DWORD style;
		DWORD exStyle;
	};

	static WindowStyles WindowStylesFromType( WindowType type )
	{
		switch ( type )
		{
			case WindowType::Main: return WindowStyles{ WS_OVERLAPPEDWINDOW, WS_EX_NOREDIRECTIONBITMAP | WS_EX_APPWINDOW };
			case WindowType::Popup: return WindowStyles{ WS_POPUP, WS_EX_NOREDIRECTIONBITMAP };
			case WindowType::Child: return WindowStyles{ WS_CHILDWINDOW, WS_EX_NOREDIRECTIONBITMAP };
			default: return WindowStyles{ 0, 0 };
		}
	}

public:
	Impl( Window& self, WindowType type, graphics::Device& device, const RectPx& rcPx, Window* parentWindow )
		: _self{ self }
		, _type{ type }
	{
		RegisterOnce();

		auto styles = WindowStylesFromType( type );
		HWND parentHwnd = nullptr;

		if ( parentWindow && parentWindow->_impl )
			parentHwnd = parentWindow->_impl->_hwnd;

		_hwnd = ::CreateWindowExW( styles.exStyle, ClassName(), nullptr, styles.style,
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

	RectPx GetRect() const
	{
		RECT rc{ 0, 0, 0, 0 };
		::GetClientRect( _hwnd, &rc );
		::MapWindowRect( _hwnd, nullptr, &rc );
		return RectPx{ rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top };
	}

	void Show()
	{
		::ShowWindow( _hwnd, SW_SHOW );
	}

	void Redraw( WindowRedraw redraw )
	{
		::RedrawWindow( _hwnd, nullptr, nullptr, redraw == WindowRedraw::Invalidate ? RDW_INVALIDATE : RDW_UPDATENOW );
	}

	void Move( const RectPx& rcPx )
	{
		::SetWindowPos( _hwnd, nullptr, rcPx.x, rcPx.y, rcPx.w, rcPx.h, SWP_NOACTIVATE | SWP_NOZORDER );
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
			return 0;
		}

		LRESULT result = 0;
		if ( ::DwmDefWindowProc( _hwnd, message, wParam, lParam, &result ) )
		{
			return result;
		}

		switch ( message )
		{
			case WM_ACTIVATE:
			{
				if ( !_activated && _type == WindowType::Main )
				{
					MARGINS margins{ 1,1,1,1 };
					auto hr = ::DwmExtendFrameIntoClientArea( _hwnd, &margins );
					if ( SUCCEEDED( hr ) )
					{
						::SetWindowPos( _hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED );
					}
					_activated = true;
					return 0;
				}
			} break;
			case WM_ERASEBKGND:
			{
				return 1;
			} break;
			case WM_NCCALCSIZE:
			{
				if ( wParam == TRUE && _type == WindowType::Main )
				{
					return 0;
				}
			} break;
			case WM_NCHITTEST:
			{
				int x = GET_X_LPARAM( lParam );
				int y = GET_Y_LPARAM( lParam );
				
				auto rect = GetRect();

				if ( x > rect.x&& x < rect.x + rect.w )
				{
					if ( y > rect.y&& y < std::min( rect.y + rect.h, rect.y + 40 ) )
					{
						return HTCAPTION;
					}
				}

				//return HTCLIENT;

				//return HTNOWHERE;
			} break;
			case WM_PAINT:
			{
				_deviceContext->BeginDraw( _hwnd );

				if ( _self.OnDraw )
					_self.OnDraw( _self, *_deviceContext );
				
				_deviceContext->EndDraw();

				return 0;
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

				if ( _self.OnMouse )
				{
					const auto& mouse = mapMessageToMouse.at( message );
					if ( mouse.state == MouseState::Down )
					{
						::SetCapture( _hwnd );
					}
					else if ( mouse.state == MouseState::Up )
					{
						::ReleaseCapture();
					}
					_self.OnMouse( _self, mouse.state, mouse.button, PointPx{ x, y } );
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

		wc.style = CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = ProgramInstance();
		wc.hIcon = nullptr;
		wc.hCursor = ::LoadCursorW( nullptr, reinterpret_cast< LPCWSTR >( IDC_ARROW ) );
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = ClassName();
		wc.hIconSm = nullptr;

		::RegisterClassExW( &wc );
	}
};

Window::Window( WindowType type, const RectPx& rcPx, Window* parentWindow )
{
	_impl.reset( new Impl{ *this, type, Application::Instance()->GetDevice(), rcPx, parentWindow } );
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

RectPx Window::GetRect() const
{
	return _impl->GetRect();
}

void Window::Show()
{
	_impl->Show();
}

void Window::Redraw( WindowRedraw redraw )
{
	_impl->Redraw( redraw );
}

void Window::Move( const RectPx& rcPx )
{
	_impl->Move( rcPx );
}

class Application::Impl
{
private:
	std::unique_ptr<graphics::Device> _device;
public:
	Impl()
	{
		_device = graphics::dx::CreateDevice();
	}

	graphics::Device& GetDevice()
	{
		return *_device.get();
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

graphics::Device& Application::GetDevice()
{
	return _impl->GetDevice();
}

float GetSystemDpi()
{
	return static_cast< float >( ::GetDpiForSystem() );
}

} // namespace directui

