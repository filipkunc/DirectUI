#pragma once

#include "Message.h"

namespace graphics
{

class DeviceContext;

}

namespace directui
{

class DrawMessage : public Message
{
private:
	graphics::DeviceContext& _deviceContext;
public:
	static const char* Name() { return "Draw"; }

	DrawMessage( Window& window, graphics::DeviceContext& deviceContext )
		: Message{ Name(), window }
		, _deviceContext{ deviceContext }
	{
	}

	graphics::DeviceContext& GetDeviceContext() const { return _deviceContext; }
};

} // namespace directui
