#pragma once

#include "CoreTypes.h"

#include <memory> // for std::unique_ptr
#include <string> // for std::wstring
#include <functional> // for std::function

namespace graphics 
{ 
class Device;
class DeviceContext;
}

namespace directui
{

using String = std::wstring;

enum class WindowType
{
	Main,
	Popup,
	Child
};

enum class WindowRedraw
{
	Invalidate,
	Now
};

enum class MouseButton
{
	None,
	Left,
	Middle,
	Right,
	Other
};

enum class MouseState
{
	Down,
	Up,
	Move,
	DoubleClick
};

class Window;

using DrawCallback = std::function<void( Window& window, graphics::DeviceContext& )>;
using MouseCallback = std::function<void( Window& window, MouseState state, MouseButton button, PointPx position )>;

class Window
{
private:
	friend class Application;

	class Impl;
	std::unique_ptr<Impl> _impl;

public:
	DrawCallback OnDraw;
	MouseCallback OnMouse;

public:
	Window( WindowType type, const RectPx& rcPx, Window* parentWindow );
	~Window();

	Handle GetHandle() const;
	float GetDpi() const;
	RectPx GetRect() const;
	
	void Show();
	void Redraw( WindowRedraw redraw = WindowRedraw::Invalidate );

	void Move( const RectPx& rcPx );
};

float GetSystemDpi();

} //namespace directui
