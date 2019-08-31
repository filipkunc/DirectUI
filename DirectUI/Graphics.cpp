#include "Graphics.h"

namespace graphics
{

class Canvas::Impl
{

};

Canvas::Canvas()
{
	_impl.reset( new Impl{} );
}

Canvas::~Canvas()
{
	_impl.reset();
}

void Canvas::BeginPaint()
{

}

void Canvas::EndPaint()
{

}

void Canvas::Clear( Color color )
{

}

void Canvas::FillSolidRect( Color color, Rect rect )
{

}

} // namespace graphics
