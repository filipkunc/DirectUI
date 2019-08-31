#pragma once

#include <memory> // for std::unique_ptr
#include <string> // for std::wstring

namespace directui
{

using String = std::wstring;

struct Rect
{
	int x, y, w, h;

	Rect() : x{ 0 }, y{ 0 }, w{ 0 }, h{ 0 } {}
	Rect( int x, int y, int w, int h ) : x{ x }, y{ y }, w{ w }, h{ h } {}
};

enum class WindowType
{
	Main,
	Popup,
	Child
};

class Window
{
private:
	friend class Application;

	class Impl;
	std::unique_ptr<Impl> _impl;
public:
	Window( WindowType type, const String& name, const Rect& rc, Window* parentWindow = nullptr );
	~Window();

	void Show();
};

}
