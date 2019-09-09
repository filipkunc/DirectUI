#include "Window.h"
#include "Application.h"
#include "Message.h"
#include "DrawMessage.h"
#include "MouseMesage.h"
#include "Graphics.h"
#include "DxGraphics.h"

int main()
{
	using namespace directui;
	using namespace graphics;

	Application app;

	auto device = dx::CreateDevice();

	Window mainWindow{ WindowType::Main, *device, L"Test App", RectPx{ 100, 100, 640, 480 }, nullptr, 
		[] ( const Message& message ) {
			if ( auto dm = message.As<DrawMessage>() )
			{
				auto& dc = dm->GetDeviceContext();
				dc.Clear( ColorF{ 0.8f, 0.8f, 0.8f, 1 } );
			}
		}	
	};
	mainWindow.Show();

	Window popupWindow{ WindowType::Popup, *device, L"Test Popup", RectPx{ 40, 40, 100, 30 }, nullptr,
		[] ( const Message& message ) {
			if ( auto dm = message.As<DrawMessage>() )
			{
				auto& dc = dm->GetDeviceContext();
				dc.Clear( ColorF{ 0.8f, 0.84f, 0.95f, 1 } );
			}
		}
	};
	popupWindow.Show();

	PointPx mousePos;

	auto textFormat = device->CreateTextFormat( L"Calibri", 18.0f );
	auto textLayout = device->CreateTextLayout( L"Just some text to see!", *textFormat, SizeF{ 150.0f, 50.0f } );

	Window childWindow{ WindowType::Child, *device, L"Test Child", RectPx{ 50, 50, 200, 150 }, &mainWindow,
		[&childWindow, &mousePos, &textLayout] ( const Message& message ) {
			if ( auto dm = message.As<DrawMessage>() )
			{
				auto& dc = dm->GetDeviceContext();
				dc.FillSolidRect( ColorF{ 1, 0.8f, 0.8f, 1 }, RectF{ 0, 0, 200, 150 } );
				dc.DrawSolidRect( ColorF{ 1, 0, 0, 1 }, RectF{ 0, 0, 200, 150 } );
				dc.FillSolidRect( ColorF{ 0, 0.9f, 0, 0.3f }, RectF{ static_cast< float >( mousePos.x ) - 5, static_cast< float >( mousePos.y ) - 5, 10, 10 } );
				auto textBrush = dc.CreateSolidBrush( ColorF{ 0, 0, 0, 1 } );
				dc.DrawTextLayout( *textLayout, *textBrush, PointF{ 10, 10 } );

			}
			else if ( auto mm = message.As<MouseMessage>() )
			{
				mousePos = mm->GetPosition();
				childWindow.Redraw();
			}
		}
	};

	childWindow.Show();

	return app.Run( mainWindow );
}
