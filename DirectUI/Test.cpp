#include "Window.h"
#include "Application.h"
#include "Graphics.h"
#include "Dpi.h"

using namespace directui;
using namespace graphics;

void DrawThinBorder( DeviceContext& dc )
{
	auto rect = dc.GetDrawRect();
	auto strokeWidth = 0.5f;
	rect.x += strokeWidth;
	rect.y += strokeWidth;
	rect.w -= strokeWidth * 2.0f;
	rect.h -= strokeWidth * 2.0f;
	dc.DrawSolidRect( ColorF{ 0x5D5D63, 1 }, rect, strokeWidth );
}

enum class MenuState
{
	Normal,
	Active,
	Pressed
};

class MenuItem
{
private:
	String _text;
	RectF _rect;
	std::unique_ptr<TextLayout> _textLayout;
	MenuState _state{ MenuState::Normal };
	std::unique_ptr<Window> _popup;
public:
	MenuItem( const String& text ) : _text{ text }, _rect{ 0, 0, 60, 20 }
	{
		auto& device = Application::Instance()->GetDevice();
		auto textFormat = device.CreateTextFormat( L"Segoe UI", 12 );
		_textLayout = device.CreateTextLayout( text, *textFormat, SizeF{ _rect.w, _rect.h } );
		_textLayout->SetTextAlignment( TextAlignment::Center );
		_textLayout->SetParagraphAlignment( ParagraphAlignment::Center );
	}

	RectF GetRect() const { return _rect; }
	void SetRect( const RectF& rect ) { _rect = rect; }

	MenuState GetState() const { return _state; }
	void SetState( MenuState state ) { _state = state; }

	/*void HandleMouse( const MouseMessage& mm, float dpi )
	{
		if ( mm.GetState() == MouseState::Down )
		{
			auto windowRect = mm.GetWindow().GetRect();
			auto popupRect = ConvertRect( RectF{ _rect.x, _rect.y + _rect.h, 200, 200 }, dpi );
			popupRect.x += windowRect.x;
			popupRect.y += windowRect.y;
			_popup.reset( new Window( WindowType::Popup, L"Popup", popupRect, nullptr, 
				[] ( const Message& message ) {
					if ( auto dm = message.As<DrawMessage>() )
					{
						auto& dc = dm->GetDeviceContext();
						dc.Clear( ColorF{ 0x1B1B1C, 1 } );
						DrawThinBorder( dc );
					}
				} ) );
			_popup->Show();
		}
	}*/

	void Draw( DeviceContext& dc )
	{
		auto brush = dc.CreateSolidBrush( ColorF{ 0x22'55'ff, 1 } );
		switch ( _state )
		{
			case MenuState::Active:		dc.FillSolidRect( ColorF{ 0x3E3E40, 1 }, _rect );	break;
			case MenuState::Pressed:	dc.FillSolidRect( ColorF{ 0x1B1B1C, 1 }, _rect );	break;
			default:					dc.DrawSolidRect( ColorF{ 0x3E3E40, 1 }, _rect );	break;
		}
		dc.DrawTextLayout( *_textLayout, *brush, PointF{ _rect.x, _rect.y } );
	}
};

class MenuBar
{
private:
	std::vector<MenuItem> _items;
public:
	void Append( MenuItem&& item )
	{
		_items.push_back( std::move( item ) );
	}

	void Layout()
	{
		PointF pos{ 0, 0 };
		for ( auto& item : _items )
		{
			auto rect = item.GetRect();
			rect.x = pos.x;
			rect.y = pos.y;
			item.SetRect( rect );
			pos.x += rect.w;
		}
	}

	/*void HandleMouse( const MouseMessage& mm, float dpi )
	{
		auto mousePos = ConvertPoint( mm.GetPosition(), dpi );
		MenuItem* activeItem = nullptr;
		for ( auto& item : _items )
		{
			item.SetState( MenuState::Normal );
			if ( activeItem == nullptr && item.GetRect().HasPoint( mousePos ) )
			{
				activeItem = &item;
			}
		}

		if ( activeItem )
		{
			activeItem->SetState( MenuState::Active );
			activeItem->HandleMouse( mm, dpi );
		}
	}*/

	void Draw( DeviceContext& dc )
	{
		for ( auto& item : _items )
		{
			item.Draw( dc );
		}
	}
};

int main()
{
	Application app;

	MenuBar menuBar;
	menuBar.Append( MenuItem{ L"File" } );
	menuBar.Append( MenuItem{ L"Edit" } );
	menuBar.Append( MenuItem{ L"View" } );
	menuBar.Append( MenuItem{ L"Project" } );
	menuBar.Layout();

	PointPx dragStartPosPx;
	bool isDragging{ false };

	Window mainWindow{ WindowType::Main, ConvertRect( RectF{ 200, 200, 640, 480 }, GetSystemDpi() ), nullptr };

	mainWindow.OnDraw = [&menuBar] ( Window& w, DeviceContext& dc ) {
		dc.Clear( ColorF{ 0x2D2D30, 0.1f } );
		menuBar.Draw( dc );
		DrawThinBorder( dc );
	};

	mainWindow.OnMouse = [&menuBar] ( Window& w, MouseState state, MouseButton button, PointPx position ) {

	};

		//[&] ( const Message& message ) {
		//	if ( auto dm = message.As<DrawMessage>() )
		//	{
		//		auto& dc = dm->GetDeviceContext();
		//		dc.Clear( ColorF{ 0x2D2D30, 1 } );
		//		menuBar.Draw( dc );
		//		DrawThinBorder( dc );
		//	}
		//	else if ( auto mm = message.As<MouseMessage>() )
		//	{
		//		auto mousePosPx = mm->GetPosition();
		//		menuBar.HandleMouse( *mm, mainWindow.GetDpi() );
		//		mainWindow.Redraw();

		//		/*if ( mm->GetState() == MouseState::Down )
		//		{
		//			dragStartPosPx = mousePosPx;
		//			isDragging = true;
		//		}
		//		else if ( mm->GetState() == MouseState::Move && isDragging )
		//		{
		//			auto rc = mainWindow.GetRect();
		//			rc.x += mousePosPx.x - dragStartPosPx.x;
		//			rc.y += mousePosPx.y - dragStartPosPx.y;
		//			mainWindow.Move( rc );
		//		}
		//		else if ( mm->GetState() == MouseState::Up )
		//		{
		//			isDragging = false;
		//		}*/
		//	}
		//}
	mainWindow.Show();
	
	return app.Run( mainWindow );
}
