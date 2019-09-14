#pragma once

#include "CoreTypes.h"

namespace directui
{

constexpr float k_DefaultDpi = 96.0f;

graphics::PointF ConvertPoint( const PointPx& pointPx, float dpi );
graphics::SizeF ConvertSize( const SizePx& sizePx, float dpi );
graphics::RectF ConvertRect( const RectPx& rectPx, float dpi );

PointPx ConvertPoint( const graphics::PointF& pointF, float dpi );
SizePx ConvertSize( const graphics::SizeF& sizeF, float dpi );
RectPx ConvertRect( const graphics::RectF& rectF, float dpi );

} // namespace directui

