#include "Window.h"
#include "Application.h"
#include "Message.h"
#include "DrawMessage.h"
#include "Graphics.h"
#include "DxGraphics.h"

int main()
{
	directui::Application app;

	auto device = graphics::dx::CreateDevice();

	directui::Window mainWindow{ directui::WindowType::Main, *device, L"Test App", directui::Rect{ 100, 100, 640, 480 }, nullptr, 
		[] ( const directui::Message& message ) {
			if ( auto dm = message.As<directui::DrawMessage>() )
			{
				auto& dc = dm->GetDeviceContext();
				dc.Clear( graphics::Color{ 0.8f, 0.8f, 0.8f, 1 } );
			}
		}	
	};
	mainWindow.Show();

	directui::Window popupWindow{ directui::WindowType::Popup, *device, L"Test Popup", directui::Rect{ 40, 40, 100, 30 }, nullptr,
		[] ( const directui::Message& message ) {
			if ( auto dm = message.As<directui::DrawMessage>() )
			{
				auto& dc = dm->GetDeviceContext();
				dc.Clear( graphics::Color{ 0.8f, 0.84f, 0.95f, 1 } );
			}
		}
	};
	popupWindow.Show();

	directui::Window childWindow{ directui::WindowType::Child, *device, L"Test Child", directui::Rect{ 50, 50, 80, 20}, &mainWindow,
		[] ( const directui::Message& message ) {
			if ( auto dm = message.As<directui::DrawMessage>() )
			{
				auto& dc = dm->GetDeviceContext();
				dc.FillSolidRect( graphics::Color{ 1, 0, 0, 1 }, graphics::Rect{ 0, 0, 80, 20 } );
				dc.FillSolidRect( graphics::Color{ 0, 0.6f, 0, 0.3f }, graphics::Rect{ 20, 0, 80, 20 } );
			}
		}
	};

	childWindow.Show();

	return app.Run( mainWindow );
}
