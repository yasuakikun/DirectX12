#include <Windows.h>
#include "Application.h"
#include "Dx12Wrapper.h"

constexpr int  WINDOW_WIDTH = 1280;
constexpr int  WINDOW_HEIGHT = 720;


HWND _hwnd;

int windowWidth;
int windowHeight;

//コールバック関数
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) 
	{															//ウィンドウが破棄されたら呼ばれます 
		PostQuitMessage(0);										//OSに対して終わると伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);			//規定の処理を行う 
}


void Application::InitWindow()
{
	WNDCLASSEX  ws = {};
	ws.cbSize = sizeof(WNDCLASSEX);
	ws.lpfnWndProc = (WNDPROC)WindowProcedure;//コールバックの指定
	ws.lpszClassName = L"DirectX12";//アプリケーション名
	ws.hInstance = GetModuleHandle(0);//ハンドルの指定
	RegisterClassEx(&ws);//アプリクラス

	RECT wrc = { 0, 0, windowWidth, windowHeight };//ウィンドウサイズを決める
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//ウィンドウのサイズを補正する


	_hwnd = CreateWindow(ws.lpszClassName,
		L"DirectX12",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowWidth,
		windowHeight,
		nullptr,//親ウィンドウハンドル 
		nullptr,//メニューハンドル 
		ws.hInstance,//呼び出しアプリケーションハンドル 
		nullptr);

	_wrapper.reset(new Dx12Wrapper(ws.hInstance, _hwnd));

}


Size Application::GetWindowSize() const
{
	return Size(windowWidth, windowHeight);
}

Application::Application()
{
	windowWidth = WINDOW_WIDTH;
	windowHeight = WINDOW_HEIGHT;
}

Application::~Application()
{
}

void Application::Initialize()
{
#ifdef DEBUG
	//デバッグレイヤー
	{
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnbleDebugLayer();
			debugController->Release();
		}
	}
#endif // DEBUG

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	InitWindow();
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
		_wrapper->Update();
	}
}

void Application::Tarminate()
{
	WNDCLASSEX  w = {};
	UnregisterClass(L"DirectX12", GetModuleHandle(0));
	CoUninitialize();

	//UnregisterClass(w.lpszClassName, w.hInstance);
}

HWND Application::GetWindowHandle()const
{
	return _hwnd;
}


