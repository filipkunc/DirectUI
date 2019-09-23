#pragma once

#include "CoreTypes.h"
#include "Message.h"

namespace directui
{

enum class MouseButton
{
	None,
	Left,
	Middle,
	Right,
	Other
};

enum class MouseState
{
	Down,
	Up,
	Move,
	DoubleClick
};

class MouseMessage : public Message
{
private:
	MouseState _state;
	MouseButton _button;
	PointPx _position;
public:
	static const char* Name() { return "Mouse"; }

	MouseMessage( Window& window, MouseState state, MouseButton button, PointPx position )
		: Message{ Name(), window }
		, _state{ state }
		, _button{ button }
		, _position{ position }
	{
	}

	MouseState GetState() const { return _state; }
	MouseButton GetButton() const { return _button; }
	PointPx GetPosition() const { return _position; }
};

} // namespace directui
