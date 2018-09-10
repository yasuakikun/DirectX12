#include "Dx12Wrapper.h"
#include "Application.h"
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")


Dx12Wrapper::Dx12Wrapper(HINSTANCE h, HWND _hwnd)
{
		D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0 ,
		D3D_FEATURE_LEVEL_11_1 ,
		D3D_FEATURE_LEVEL_11_0,
	};

		D3D_FEATURE_LEVEL level;
		for (auto l : levels) {
			if(D3D12CreateDevice(nullptr, l, IID_PPV_ARGS(&_dev)) == S_OK)
			{
				level = l;
				break;
			}
		}

		auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_fac));


		DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};

		auto wsize = Application::Instance().GetWindowSize();
		swapchainDesc.Width = wsize.w;
		swapchainDesc.Width = wsize.h;
		swapchainDesc.BufferCount = 2;
		swapchainDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
		swapchainDesc.Stereo = false;
		swapchainDesc.SampleDesc.Count= 1;
		swapchainDesc.SampleDesc.Quality = 0;
		swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
		/*swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_FORCE_DWORD;*/
		swapchainDesc.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		result = _fac->CreateSwapChainForHwnd(_comqueue,
			Application::Instance().GetWindowHandle(),
			&swapchainDesc,
			nullptr,
			nullptr,
			(IDXGISwapChain1**)(&_swapchain));

}


Dx12Wrapper::~Dx12Wrapper()
{
}
