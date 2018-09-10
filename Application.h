#pragma once
#include <memory>

struct Size
{
	int w;
	int h;
};
;


class Dx12Wrapper;
class Application
{
private:
	std::shared_ptr<Dx12Wrapper> _wrapper;
	int  WINDOW_WIDTH;
	int  WINDOW_HEIGHT;
	Application();
	Application(const Application&);
	Application& operator=(const Application&) {};

public:
	static Application& Instance() {
		static Application Instance;
		return Instance;
	}
	Size GetWindowSize();

	~Application();

	void Initialize();
	void Run();
	void Tarminate();

	void InitWindow();

	HWND GetWindowHandle()const;

};

