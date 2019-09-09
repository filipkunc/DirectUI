#pragma once

#include "CoreTypes.h"

#include <memory> // for std::unique_ptr
#include <string> // for std::wstring
#include <functional> // for std::function

namespace graphics { class Device; }

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

class Message;

using MessageHandler = std::function<void( const Message& message )>;

class Window
{
private:
	friend class Application;

	class Impl;
	std::unique_ptr<Impl> _impl;
public:
	Window( WindowType type, graphics::Device& device, const String& name, const RectPx& rcPx, Window* parentWindow, MessageHandler messageHandler );
	~Window();

	Handle GetHandle() const;

	void Show();
	void Redraw( WindowRedraw redraw = WindowRedraw::Invalidate );
};

}
