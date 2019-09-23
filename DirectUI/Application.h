#pragma once

#include <memory> // for std::unique_ptr

namespace graphics
{
class Device;
}

namespace directui
{

class Window;

class Application
{
private:
	class Impl;
	std::unique_ptr<Impl> _impl;
public:
	Application();
	~Application();

	static Application*& Instance();

	int Run( Window& window );
	graphics::Device& GetDevice();
};

} // namespace directui
