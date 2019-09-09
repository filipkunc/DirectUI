#include "Graphics.h"
#include "Application.h"

namespace graphics
{

void DeviceContext::FillSolidRect( const ColorF& color, const RectF& rect )
{
	auto brush = CreateSolidBrush( color );
	FillRect( *brush, rect );
}

void DeviceContext::DrawSolidRect( const ColorF& color, const RectF& rect, float strokeWidth )
{
	auto brush = CreateSolidBrush( color );
	DrawRect( *brush, rect, strokeWidth );
}

} // namespace graphics

