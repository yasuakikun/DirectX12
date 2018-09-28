#include <Windows.h>
#include "Application.h"
#include "Dx12Wrapper.h"

constexpr int  WINDOW_WIDTH = 1280;
constexpr int  WINDOW_HEIGHT = 720;


HWND _hwnd;

int windowWidth;
int windowHeight;

//�R�[���o�b�N�֐�
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) 
	{															//�E�B���h�E���j�����ꂽ��Ă΂�܂� 
		PostQuitMessage(0);										//OS�ɑ΂��ďI���Ɠ`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);			//�K��̏������s�� 
}


void Application::InitWindow()
{
	WNDCLASSEX  ws = {};
	ws.cbSize = sizeof(WNDCLASSEX);
	ws.lpfnWndProc = (WNDPROC)WindowProcedure;//�R�[���o�b�N�̎w��
	ws.lpszClassName = L"DirectX12";//�A�v���P�[�V������
	ws.hInstance = GetModuleHandle(0);//�n���h���̎w��
	RegisterClassEx(&ws);//�A�v���N���X

	RECT wrc = { 0, 0, windowWidth, windowHeight };//�E�B���h�E�T�C�Y�����߂�
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//�E�B���h�E�̃T�C�Y��␳����


	_hwnd = CreateWindow(ws.lpszClassName,
		L"DirectX12",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowWidth,
		windowHeight,
		nullptr,//�e�E�B���h�E�n���h�� 
		nullptr,//���j���[�n���h�� 
		ws.hInstance,//�Ăяo���A�v���P�[�V�����n���h�� 
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
	//�f�o�b�O���C���[
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
			TranslateMessage(&msg);//���z�L�[�֘A�̕ϊ� 
			DispatchMessage(&msg);//��������Ȃ��������b�Z�[�W��OS�ɕԂ�
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


