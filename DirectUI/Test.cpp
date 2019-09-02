#include "Window.h"
#include "Application.h"
#include "Message.h"
#include "PaintMessage.h"
#include "Graphics.h"

int main()
{
	directui::Application app;

	directui::Window mainWindow{ directui::WindowType::Main, L"Test App", directui::Rect{ 100, 100, 640, 480 }, nullptr, 
		[] ( const directui::Message& message ) {
			if ( auto paintMessage = message.As<directui::PaintMessage>() )
			{
				auto& canvas = paintMessage->GetCanvas();
				canvas.Clear( graphics::Color{ 0.8f, 0.8f, 0.8f, 1 } );
			}
		}	
	};
	mainWindow.Show();

	directui::Window popupWindow{ directui::WindowType::Popup, L"Test Popup", directui::Rect{ 40, 40, 100, 30 }, nullptr,
		[] ( const directui::Message& message ) {
			if ( auto paintMessage = message.As<directui::PaintMessage>() )
			{
				auto& canvas = paintMessage->GetCanvas();
				canvas.Clear( graphics::Color{ 0.8f, 0.84f, 0.95f, 1 } );
			}
		}
	};
	popupWindow.Show();

	directui::Window childWindow{ directui::WindowType::Child, L"Test Child", directui::Rect{ 50, 50, 80, 20}, &mainWindow,
		[] ( const directui::Message& message ) {
			if ( auto paintMessage = message.As<directui::PaintMessage>() )
			{
				auto& canvas = paintMessage->GetCanvas();
				canvas.FillSolidRect( graphics::Color{ 1, 0, 0, 1 }, graphics::Rect{ 0, 0, 80, 20 } );
				canvas.FillSolidRect( graphics::Color{ 0, 0.6f, 0, 0.3f }, graphics::Rect{ 20, 0, 80, 20 } );
			}
		}
	};

	childWindow.Show();

	return app.Run( mainWindow );
}
