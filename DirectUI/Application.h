#pragma once

#include <memory> // for std::unique_ptr

namespace graphics::dx { class Device; }

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

	graphics::dx::Device& GetDevice();
	int Run( Window& window );
};

} // namespace directui
