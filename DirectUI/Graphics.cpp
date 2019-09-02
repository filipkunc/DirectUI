#include "Graphics.h"
#include "DxGraphics.h"
#include "Application.h"

namespace graphics
{

class Canvas::Impl
{
private:
	HWND _hwnd;
	HDC _hdc;
	PAINTSTRUCT _ps;
	dx::DeviceContext _deviceContext;
public:
	Impl()
		: _hwnd{ nullptr }
		, _hdc{ nullptr }
		, _ps{}
		, _deviceContext{ directui::Application::Instance()->GetDevice() }
	{
	}

	~Impl()
	{

	}

	void BeginPaint( directui::Handle windowHandle )
	{
		_hwnd = static_cast< HWND >( windowHandle );
		_deviceContext.Resize( _hwnd );
		_hdc = ::BeginPaint( _hwnd, &_ps );
		_deviceContext.BeginDraw();
	}
	
	RECT ConvertRect( Rect rc )
	{
		return RECT{
			static_cast< LONG >( rc.x ),
			static_cast< LONG >( rc.y ),
			static_cast< LONG >( rc.x + rc.w ),
			static_cast< LONG >( rc.y + rc.h )
		};
	}

	COLORREF ConvertColor( Color color )
	{
		auto r = static_cast< UINT >( color.r * 255.0f );
		auto g = static_cast< UINT >( color.g * 255.0f );
		auto b = static_cast< UINT >( color.b * 255.0f );
		return RGB( r, g, b );
	}

	void EndPaint()
	{
		_deviceContext.EndDraw();
		::EndPaint( _hwnd, &_ps );
	}

	void Clear( Color color )
	{
		/*auto gdiBrush = ::CreateSolidBrush( ConvertColor( color ) );
		::FillRect( _hdc, &_ps.rcPaint, gdiBrush );
		::DeleteObject( gdiBrush );*/

		_deviceContext._d2dContext->Clear( D2D1::ColorF( color.r, color.g, color.b, color.a ) );
	}

	void FillSolidRect( Color color, Rect rect )
	{
		/*auto gdiBrush = ::CreateSolidBrush( ConvertColor( color ) );
		auto gdiRect = ConvertRect( rect );
		::FillRect( _hdc, &gdiRect, gdiBrush );
		::DeleteObject( gdiBrush );*/

		dx::wrl::ComPtr<ID2D1SolidColorBrush> brush;
		_deviceContext._d2dContext->CreateSolidColorBrush(
			D2D1::ColorF( color.r, color.g, color.b, color.a ),
			&brush );
		_deviceContext._d2dContext->FillRectangle(
			D2D1::RectF( rect.x, rect.y, rect.x + rect.w, rect.y + rect.h ),
			brush.Get() );
	}
};

Canvas::Canvas()
{
	_impl.reset( new Impl{} );
}

Canvas::~Canvas()
{
	_impl.reset();
}

void Canvas::BeginPaint( directui::Handle windowHandle )
{
	_impl->BeginPaint( windowHandle );
}

void Canvas::EndPaint()
{
	_impl->EndPaint();
}

void Canvas::Clear( Color color )
{
	_impl->Clear( color );
}

void Canvas::FillSolidRect( Color color, Rect rect )
{
	_impl->FillSolidRect( color, rect );
}

} // namespace graphics

