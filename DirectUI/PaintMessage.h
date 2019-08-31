#pragma once

#include "Message.h"

namespace graphics
{

class Canvas;

}

namespace directui
{

class PaintMessage : public Message
{
private:
	graphics::Canvas& _canvas;
public:
	static const char* Name() { return "Paint"; }

	PaintMessage( graphics::Canvas& canvas )
		: Message{ Name() }
		, _canvas{ canvas }
	{
	}

	graphics::Canvas& GetCanvas() const { return _canvas; }
};

} // namespace directui
