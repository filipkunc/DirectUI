#pragma once

#include "CoreTypes.h"
#include <memory> // for std::unique_ptr
#include <functional> // for std::function
#include <string> // for std::wstring

namespace graphics
{

using String = std::wstring;

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

class DeviceContext;

class Brush : public directui::NamedBase
{
public:
	using NamedBase::NamedBase;
};

class TextFormat : public directui::NamedBase
{
public:
	using NamedBase::NamedBase;
};

class TextLayout : public directui::NamedBase
{
public:
	using NamedBase::NamedBase;
};

class Device
{
public:
	virtual ~Device() {}

	virtual std::unique_ptr<DeviceContext> CreateDeviceContext() = 0;
	virtual std::unique_ptr<TextFormat> CreateTextFormat( const String& fontFamily, float height ) = 0;
	virtual std::unique_ptr<TextLayout> CreateTextLayout( const String& text, const TextFormat& format, const SizeF& sizeFit ) = 0;
};

class DeviceContext
{
public:
	virtual ~DeviceContext() {}

	virtual std::unique_ptr<Brush> CreateSolidBrush( const ColorF& color ) = 0;

	virtual void BeginDraw( directui::Handle windowHandle ) = 0;
	virtual void EndDraw() = 0;

	virtual void Clear( const ColorF& color ) = 0;
	virtual void FillRect( Brush& brush, const RectF& rect ) = 0;
	virtual void DrawRect( Brush& brush, const RectF& rect, float strokeWidth = 1.0f ) = 0;
	virtual void DrawTextLayout( const TextLayout& layout, Brush& brush, const PointF& position ) = 0;

	void FillSolidRect( const ColorF& color, const RectF& rect );
	void DrawSolidRect( const ColorF& color, const RectF& rect, float strokeWidth = 1.0f );
};

} // namespace graphics