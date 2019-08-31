#pragma once

#include "CoreTypes.h"
#include <memory> // for std::unique_ptr

namespace graphics
{

struct Point
{
	float x, y;
	Point() : x{ 0 }, y{ 0 } {}
	Point( float x, float y ) : x{ x }, y{ y } {}
};

struct Rect
{
	float x, y, w, h;
	Rect() : x{ 0 }, y{ 0 }, w{ 0 }, h{ 0 } {}
	Rect( float x, float y, float w, float h ) : x{ x }, y{ y }, w{ w }, h{ h } {}
};

struct Color
{
	float r, g, b, a;
	Color() : r{ 0 }, g{ 0 }, b{ 0 }, a{ 0 } {}
	Color( float r, float g, float b, float a ) : r{ r }, g{ g }, b{ b }, a{ a } {}
};

class Canvas
{
private:
	class Impl;
	std::unique_ptr<Impl> _impl;
public:
	Canvas();
	~Canvas();

	void BeginPaint( directui::Handle windowHandle );
	void EndPaint();

	void Clear( Color color );
	void FillSolidRect( Color color, Rect rect );
};

} // namespace graphics