#pragma once

namespace directui
{

using Handle = void*;

class NamedBase
{
protected:
	const char* _name;
public:
	NamedBase( const char* name ) : _name{ name } {}
	virtual ~NamedBase() {}

	template< typename TDerived >
	TDerived* As()
	{
		if ( TDerived::Name() == _name )
		{
			return static_cast< TDerived* >( this );
		}
		return nullptr;
	}

	template< typename TDerived >
	const TDerived* As() const
	{
		if ( TDerived::Name() == _name )
		{
			return static_cast< const TDerived* >( this );
		}
		return nullptr;
	}
};

struct PointPx
{
	int x, y;
	PointPx() : x{ 0 }, y{ 0 } {}
	PointPx( int x, int y ) : x{ x }, y{ y } {}
};

struct SizePx
{
	int w, h;
	SizePx() : w{ 0 }, h{ 0 } {}
	SizePx( int w, int h ) : w{ w }, h{ h } {}
};

struct RectPx
{
	int x, y, w, h;
	RectPx() : x{ 0 }, y{ 0 }, w{ 0 }, h{ 0 } {}
	RectPx( int x, int y, int w, int h ) : x{ x }, y{ y }, w{ w }, h{ h } {}
};

} // namespace directui
