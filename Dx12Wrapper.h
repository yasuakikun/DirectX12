#pragma once
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>

class Dx12Wrapper
{
private:

	ID3D12Device * dev;

	//レンダーターゲットビューのためのヒープ
	ID3D12DescriptorHeap* rtvDescHeap;
	//テクスチャ、定数のためのヒープ
	ID3D12DescriptorHeap* rgstDescHeap;
	//テクスチャ
	D3D12_SHADER_RESOURCE_VIEW_DESC texView;

	ID3D12CommandQueue* commandQueue;
	IDXGIFactory5* dxgiFactory;
	IDXGISwapChain4* swapchain;
	ID3D12CommandAllocator* commandAllocator;
	ID3D12GraphicsCommandList* commandList;
	ID3D12Fence* fence;
	UINT64 fenceValue;
	ID3D12PipelineState* pipeline;
	ID3D12RootSignature* rootSignature;//これが最終的に欲しいオブジェクト
	ID3DBlob* signature;//ルートシグネチャをつくるための材料

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;

	std::vector<ID3D12Resource*> backBuffers;

	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;

	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW ibView;

	//頂点情報を定義し、頂点バッファを作る
	ID3D12Resource* vertexBuff;

	//コンスタントバッファ用
	ID3D12Resource* cbuff;

	void InitVertices();
	void InitShader();
	void InitTexture();

	//コンスタントバッファの初期化等
	//※テクスチャの初期化の後に呼び出す
	void InitConstants();
public:
	Dx12Wrapper(HINSTANCE h, HWND hwnd);
	~Dx12Wrapper();
	void Update();
	void ExecuteCommand();
	void WaitExecute();
};

