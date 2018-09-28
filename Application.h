#pragma once
#include <memory>
#include "Dx12Wrapper.h"

struct Size
{
	Size(){}
	Size(int inw, int inh) :w(inw), h(inh) {};
	int w;
	int h;
};

class Dx12Wrapper;
//シングルトンクラス
class Application
{
private:
	std::shared_ptr<Dx12Wrapper> _wrapper;
	void InitWindow();
	Application();
	Application(const Application&);
	void operator=(const Application&);

public:
	Size GetWindowSize()const;
	static Application& Instance() {
		static Application Instance;
		return Instance;
	}
	~Application();

	//初期化
	void Initialize();
	//ループ
	void Run();
	//後処理
	void Tarminate();

	HWND GetWindowHandle()const;
};

