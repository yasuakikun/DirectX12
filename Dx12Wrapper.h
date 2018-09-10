#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>

//ID3D12Device* _dev = nullptr;
//ID3D12CommandAllocator* _comall = nullptr;
//ID3D12GraphicsCommandList* _comlist = nullptr;
//ID3D12CommandQueue* _comqueue = nullptr;
//ID3D12Fence* _fence = nullptr;
//IDXGIFactory6* _fac = nullptr;
//IDXGISwapChain4* _swapchain = nullptr;

class Dx12Wrapper
{
private: 
	ID3D12Device * _dev;
	ID3D12CommandQueue* _comqueue;
	IDXGIFactory6* _fac;
	IDXGISwapChain4* _swapchain;

public:
	Dx12Wrapper(HINSTANCE h, HWND _hwnd);
	~Dx12Wrapper();
};

