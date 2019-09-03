#pragma once

#include "Message.h"

namespace graphics
{

class IDeviceContext;

}

namespace directui
{

class DrawMessage : public Message
{
private:
	graphics::IDeviceContext& _deviceContext;
public:
	static const char* Name() { return "Draw"; }

	DrawMessage( graphics::IDeviceContext& deviceContext )
		: Message{ Name() }
		, _deviceContext{ deviceContext }
	{
	}

	graphics::IDeviceContext& GetDeviceContext() const { return _deviceContext; }
};

} // namespace directui
