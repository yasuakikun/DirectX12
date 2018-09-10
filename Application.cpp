#include "Application.h"
#include <Windows.h>

HWND _hwnd;

constexpr int  WINDOW_WIDTH = 1280;
constexpr int  WINDOW_HEIGHT = 840;

//コールバック関数
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) 
	{																					//ウィンドウが破棄されたら呼ばれます 
		PostQuitMessage(0);																//OSに対して「もうこのアプリは終わるんや」と伝える 
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);									//規定の処理を行う 
}


Application::Application()
{
}


Size Application::GetWindowSize()
{
	return Size();
}

Application::~Application()
{
}


void Application::Initialize()
{
}

void Application::InitWindow()
{
	WNDCLASSEX  ws = {};
	ws.cbSize = sizeof(WNDCLASSEX);
	ws.lpfnWndProc = (WNDPROC)WindowProcedure;//コールバックの指定
	ws.lpszClassName = ("DirectX12");//アプリケーション名
	ws.hInstance = GetModuleHandle(0);//ハンドルの指定
	RegisterClassEx(&ws);//アプリクラス



	_hwnd = CreateWindow(ws.lpszClassName, ("DirectX12"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		nullptr,//親ウィンドウハンドル 
		nullptr,//メニューハンドル 
		ws.hInstance,//呼び出しアプリケーションハンドル 
		nullptr);

	_wrapper.reset();

}

HWND Application::GetWindowHandle() const
{
	return HWND();
}


void Application::Run()
{
	ShowWindow(_hwnd, SW_SHOW);
	MSG msg = {};
	while (true)
	{
		if(PeekMessage(&msg,nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);//仮想キー関連の変換 
			DispatchMessage(&msg);//処理されなかったメッセージをOSに返す
		}
		if (msg.message == WM_QUIT)
		{
			break;
		}
	}
}

void Application::Tarminate()
{
	WNDCLASSEX  w = {};
	UnregisterClass(w.lpszClassName, w.hInstance);
}


