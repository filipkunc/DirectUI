#include "Window.h"
#include "Application.h"
#include "Message.h"
#include "DrawMessage.h"
#include "MouseMesage.h"
#include "Graphics.h"
#include "DxGraphics.h"
#include "Dpi.h"

int main()
{
	using namespace directui;
	using namespace graphics;

	Application app;

	auto device = dx::CreateDevice();

	Window mainWindow{ WindowType::Main, *device, L"Test App", ConvertRect( RectF{ 100, 100, 640, 480 }, GetSystemDpi() ), nullptr,
		[] ( const Message& message ) {
			if ( auto dm = message.As<DrawMessage>() )
			{
				auto& dc = dm->GetDeviceContext();
				dc.Clear( ColorF{ 0.8f, 0.8f, 0.8f, 1 } );
			}
		}	
	};
	mainWindow.Show();

	Window popupWindow{ WindowType::Popup, *device, L"Test Popup", ConvertRect( RectF{ 40, 40, 100, 30 }, mainWindow.GetDpi() ), nullptr,
		[] ( const Message& message ) {
			if ( auto dm = message.As<DrawMessage>() )
			{
				auto& dc = dm->GetDeviceContext();
				dc.Clear( ColorF{ 0.8f, 0.84f, 0.95f, 1 } );
			}
		}
	};
	popupWindow.Show();

	PointPx mousePosPx;

	auto textFormat = device->CreateTextFormat( L"Calibri", 14.0f );
	auto textLayout = device->CreateTextLayout( L"Just some text to see!", *textFormat, SizeF{ 150.0f, 50.0f } );

	Window childWindow{ WindowType::Child, *device, L"Test Child", ConvertRect( RectF{ 50, 50, 200, 150 }, mainWindow.GetDpi() ), &mainWindow,
		[&childWindow, &mousePosPx, &textLayout] ( const Message& message ) {
			if ( auto dm = message.As<DrawMessage>() )
			{
				auto& dc = dm->GetDeviceContext();
				dc.FillSolidRect( ColorF{ 1, 0.8f, 0.8f, 1 }, RectF{ 0, 0, 200, 150 } );
				dc.DrawSolidRect( ColorF{ 1, 0, 0, 1 }, RectF{ 10, 10, 50, 20 } );
				auto mousePosF = ConvertPoint( mousePosPx, childWindow.GetDpi() );
				dc.FillSolidRect( ColorF{ 0, 0.9f, 0, 0.3f }, RectF{ mousePosF.x - 5, mousePosF.y - 5, 10, 10 } );
				auto textBrush = dc.CreateSolidBrush( ColorF{ 0, 0, 0, 1 } );
				dc.DrawTextLayout( *textLayout, *textBrush, PointF{ 10, 10 } );

			}
			else if ( auto mm = message.As<MouseMessage>() )
			{
				mousePosPx = mm->GetPosition();
				childWindow.Redraw();
			}
		}
	};

	childWindow.Show();

	return app.Run( mainWindow );
}
