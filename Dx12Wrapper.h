#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <wrl.h>

class PMDModel;

using namespace DirectX;

struct TransformMatrixs
{
	XMMATRIX world;	//合成前ワールド
	XMMATRIX wvp;	//WVP合成済み
};

struct Material
{
	XMFLOAT3 difuse;
};

class Dx12Wrapper
{
private:

	TransformMatrixs mat;

	std::shared_ptr<PMDModel> model;

	XMMATRIX camera;
	XMMATRIX projection;

	XMMATRIX * mappedmatrix;

	std::vector<D3D12_DESCRIPTOR_RANGE> descTblRange;

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> dev;

	//レンダーターゲットビューのためのヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescHeap;

	//テクスチャ、定数のためのヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rgstDescHeap;

	//デプスステンシルビューのヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescHeap;

	//テクスチャ
	D3D12_SHADER_RESOURCE_VIEW_DESC texView;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;	//最新 6
	IDXGISwapChain4 * swapchain;	//最新 4
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 fenceValue;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;//これが最終的に欲しいオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob> signature;//ルートシグネチャをつくるための材料

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;

	std::vector< Microsoft::WRL::ComPtr<ID3D12Resource> > backBuffers;

	Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;

	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW ibView;

	//頂点情報を定義し、頂点バッファを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuff;

	//コンスタントバッファ用
	Microsoft::WRL::ComPtr<ID3D12Resource> cbuff;

	Microsoft::WRL::ComPtr<ID3D12Resource> dsvBuff;

	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuff;

	//初期化関係

	//コマンド系初期化
	void InitCommandSet();

	//レンダーターゲットビューのためのデスクリプタヒープを作成
	void InitDescriptorHeapForRTV();

	//スワップチェーン初期化
	void InitRenderTargetViewFromSwapchain();

	//フェンスの生成
	void InitFence();

	//デプスステンシルビューの初期化
	void InitBufferforDSV();

	//デプスステンシルビューの生成
	void InitDepthView();

	//頂点バッファの初期化(インデックスバッファも)
	void InitVertices();

	//シェーダの初期化
	void InitShader();

	//レジスタ系の初期化
	void InitDescriptorHeapForRegister();

	//ルートシグネチャの初期化
	void InitRootSignature();

	//パイプラインステート初期化
	void InitPipelineState();

	//テクスチャの初期化
	void InitTexture();

	//コンスタントバッファの初期化等
	//※テクスチャの初期化の後に呼び出す
	void InitConstants();

	//マテリアルの初期化
	void InitMaterial();

	//フェンス
	void ExecuteCommand();
	void WaitExecute();

public:
	Dx12Wrapper(HINSTANCE h, HWND hwnd);
	~Dx12Wrapper();

	void Update();
};
