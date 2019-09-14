#include "Dpi.h"

#include <cmath>

namespace directui
{

graphics::PointF ConvertPoint( const PointPx& pointPx, float dpi )
{
	dpi /= k_DefaultDpi;
	return graphics::PointF
	{
		static_cast< float >( pointPx.x ) / dpi,
		static_cast< float >( pointPx.y ) / dpi,
	};
}

graphics::SizeF ConvertSize( const SizePx& sizePx, float dpi )
{
	dpi /= k_DefaultDpi;
	return graphics::SizeF
	{
		static_cast< float >( sizePx.w ) / dpi,
		static_cast< float >( sizePx.h ) / dpi,
	};
}

graphics::RectF ConvertRect( const RectPx& rectPx, float dpi )
{
	dpi /= k_DefaultDpi;
	return graphics::RectF
	{
		static_cast< float >( rectPx.x ) / dpi,
		static_cast< float >( rectPx.y ) / dpi,
		static_cast< float >( rectPx.w ) / dpi,
		static_cast< float >( rectPx.h ) / dpi,
	};
}

//------------------------------------------------------------------

PointPx ConvertPoint( const graphics::PointF& pointF, float dpi )
{
	dpi /= k_DefaultDpi;
	return PointPx
	{
		static_cast< int >( floor( pointF.x * dpi ) ),
		static_cast< int >( floor( pointF.y * dpi ) ),
	};
}

SizePx ConvertSize( const graphics::SizeF& sizeF, float dpi )
{
	dpi /= k_DefaultDpi;
	return SizePx
	{
		static_cast< int >( ceil( sizeF.w * dpi ) ),
		static_cast< int >( ceil( sizeF.h * dpi ) ),
	};
}

RectPx ConvertRect( const graphics::RectF& rectF, float dpi )
{
	dpi /= k_DefaultDpi;
	return RectPx
	{
		static_cast< int >( floor( rectF.x * dpi ) ),
		static_cast< int >( floor( rectF.y * dpi ) ),
		static_cast< int >( ceil( rectF.w * dpi ) ),
		static_cast< int >( ceil( rectF.h * dpi ) ),
	};
}

} // namespace directui

