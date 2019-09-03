#pragma once

#include <memory> // for std::unique_ptr

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
};

} // namespace directui
