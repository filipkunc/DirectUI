#include "Window.h"
#include "Application.h"

int main()
{
	directui::Application app;

	directui::Window mainWindow{ directui::WindowType::Main, L"Test App", directui::Rect{ 100, 100, 640, 480 } };
	mainWindow.Show();

	directui::Window popupWindow{ directui::WindowType::Popup, L"Test Popup", directui::Rect{ 40, 40, 100, 30 } };
	popupWindow.Show();

	directui::Window childWindow{ directui::WindowType::Child, L"Test Child", directui::Rect{ 50, 50, 80, 20}, &mainWindow };

	return app.Run( mainWindow );
}
