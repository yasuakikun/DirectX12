#include "Application.h"
#include <Windows.h>

HWND _hwnd;

constexpr int  WINDOW_WIDTH = 1280;
constexpr int  WINDOW_HEIGHT = 840;

//�R�[���o�b�N�֐�
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) 
	{																					//�E�B���h�E���j�����ꂽ��Ă΂�܂� 
		PostQuitMessage(0);																//OS�ɑ΂��āu�������̃A�v���͏I�����v�Ɠ`���� 
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);									//�K��̏������s�� 
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
	ws.lpfnWndProc = (WNDPROC)WindowProcedure;//�R�[���o�b�N�̎w��
	ws.lpszClassName = ("DirectX12");//�A�v���P�[�V������
	ws.hInstance = GetModuleHandle(0);//�n���h���̎w��
	RegisterClassEx(&ws);//�A�v���N���X



	_hwnd = CreateWindow(ws.lpszClassName, ("DirectX12"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		nullptr,//�e�E�B���h�E�n���h�� 
		nullptr,//���j���[�n���h�� 
		ws.hInstance,//�Ăяo���A�v���P�[�V�����n���h�� 
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
			TranslateMessage(&msg);//���z�L�[�֘A�̕ϊ� 
			DispatchMessage(&msg);//��������Ȃ��������b�Z�[�W��OS�ɕԂ�
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


