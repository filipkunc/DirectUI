#pragma once

#include "CoreTypes.h"
#include <memory> // for std::unique_ptr
#include <functional> // for std::function

namespace graphics
{

struct PointF
{
	float x, y;
	PointF() : x{ 0 }, y{ 0 } {}
	PointF( float x, float y ) : x{ x }, y{ y } {}
};

struct SizeF
{
	float w, h;
	SizeF() : w{ 0 }, h{ 0 } {}
	SizeF( float w, float h ) : w{ w }, h{ h } {}
};

struct RectF
{
	float x, y, w, h;
	RectF() : x{ 0 }, y{ 0 }, w{ 0 }, h{ 0 } {}
	RectF( float x, float y, float w, float h ) : x{ x }, y{ y }, w{ w }, h{ h } {}
};

struct ColorF
{
	float r, g, b, a;
	ColorF() : r{ 0 }, g{ 0 }, b{ 0 }, a{ 0 } {}
	ColorF( float r, float g, float b, float a ) : r{ r }, g{ g }, b{ b }, a{ a } {}
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

	virtual void Clear( const ColorF& color ) = 0;
	virtual void FillSolidRect( const ColorF& color, const RectF& rect ) = 0;
};

} // namespace graphics