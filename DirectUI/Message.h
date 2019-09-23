#pragma once

#include "CoreTypes.h"

namespace directui
{

class Window;

class Message : public NamedBase
{
private:
	Window& _window;

public:
	using NamedBase::NamedBase;

	Message( const char* name, Window& window )
		: NamedBase{ name }
		, _window{ window }
	{
	}

	Window& GetWindow() const { return _window; }
};

} // namespace directui
