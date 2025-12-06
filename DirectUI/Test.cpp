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

int main()
{
	Application app;

	auto textPos = PointF{ 20.0f, 20.0f };

	auto& device = app.GetDevice();
	auto textLayout = device.CreateTextLayout(L"DirectUI 👩‍🦰 Test 🎶 Application", *device.CreateTextFormat(L"Segoe UI", 58.0f), SizeF{ 600.0f, 100.0f });

	Window mainWindow{ WindowType::Main, ConvertRect( RectF{ 200, 200, 640, 480 }, GetSystemDpi() ), nullptr };

	mainWindow.OnDraw = [&] ( Window& w, DeviceContext& dc ) {
		dc.Clear( ColorF{ 0x2D2D30, 0.5f } );
		DrawThinBorder( dc );
		dc.DrawTextLayout(*textLayout, *dc.CreateSolidBrush(ColorF{ 1, 1, 1, 1 }), textPos);
	};

	mainWindow.OnMouse = [&] ( Window& w, MouseState state, MouseButton button, PointPx position ) {
		textPos = ConvertPoint(position, w.GetDpi());
		w.Redraw();
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
