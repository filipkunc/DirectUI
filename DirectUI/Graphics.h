#pragma once

#include "CoreTypes.h"
#include <memory> // for std::unique_ptr
#include <functional> // for std::function

namespace graphics
{

struct Point
{
	float x, y;
	Point() : x{ 0 }, y{ 0 } {}
	Point( float x, float y ) : x{ x }, y{ y } {}
};

struct Size
{
	float w, h;
	Size() : w{ 0 }, h{ 0 } {}
	Size( float w, float h ) : w{ w }, h{ h } {}
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

class IDeviceContext;

class IDevice
{
public:
	virtual ~IDevice() {}

	virtual std::unique_ptr<IDeviceContext> CreateDeviceContext() = 0;
};

class IDeviceContext
{
public:
	virtual ~IDeviceContext() {}

	virtual void BeginDraw( directui::Handle windowHandle ) = 0;
	virtual void EndDraw() = 0;

	virtual void Clear( Color color ) = 0;
	virtual void FillSolidRect( Color color, Rect rect ) = 0;
};

} // namespace graphics