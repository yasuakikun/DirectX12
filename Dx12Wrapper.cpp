#include "Dx12Wrapper.h"
#include "Application.h"
#include <vector>
#include <algorithm>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <d3dcompiler.h>
#include <tchar.h>
#include <random>
#include "D3Dx12.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"DirectXTex.lib")

using namespace DirectX;

struct Vertex {
	XMFLOAT3 pos;//頂点座標
				 //XMFLOAT3 normal;//法線ベクトル
	XMFLOAT2 uv;//UV値
};

void Dx12Wrapper::InitVertices() {
	//頂点情報(Nの字の順)
	Vertex vertices[] = {


	/*	XMFLOAT3(-0.5f, -1.0f, 0.0),XMFLOAT2(0,1),
		XMFLOAT3(-0.5f,  1.0f, 0.0),XMFLOAT2(0,0),
		XMFLOAT3(0.5f, -1.0f, 0.0),XMFLOAT2(1,1),
		XMFLOAT3(0.5f,  1.0f, 0.0),XMFLOAT2(1,0)*/

		XMFLOAT3(-0.0f, -10.0f, 0.0f), XMFLOAT2(0.0f, 1.0f),//左下
		XMFLOAT3(-0.0f,  10.0f, 0.0f), XMFLOAT2(0.0f, 0.0f),//左上
		XMFLOAT3(20.0f, -10.0f, 0.0f), XMFLOAT2(1.0f, 1.0f),//右下
		XMFLOAT3(20.0f,  10.0f, 0.0f), XMFLOAT2(1.0f, 0.0f),//右上

		//XMFLOAT3(0.0f, 720.0f, 0.0f), XMFLOAT2(0.0f, 1.0f),//左下
		//XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f),//左上
		//XMFLOAT3(1280.0f, 720.0f, 0.0f), XMFLOAT2(1.0f, 1.0f),//右下
		//XMFLOAT3(1280.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f),//右上
	};

	auto result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPUからGPUへ転送する用
		D3D12_HEAP_FLAG_NONE,//特別な指定なし
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),//サイズ
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuff)
	);

	D3D12_RANGE range = { 0,0 };
	Vertex* vbuffptr = nullptr;
	result = vertexBuff->Map(0, nullptr, (void**)&vbuffptr);
	std::copy(std::begin(vertices), std::end(vertices), vbuffptr);
	vertexBuff->Unmap(0, nullptr);//mapをしたら確実にUnmapする

	vbView.BufferLocation = vertexBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(Vertex);
}

void Dx12Wrapper::InitShader() {
	ID3DBlob* error = nullptr;
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	HRESULT result = S_OK;

	result = D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "vs", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vsBlob, nullptr);
	result = D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "ps", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &psBlob, nullptr);

	D3D12_DESCRIPTOR_RANGE descTblRange[2] = {};
	CD3DX12_ROOT_PARAMETER rootParam = {};
	//ここが"t0"に流すことを表している
	descTblRange[0].NumDescriptors = 1;
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descTblRange[0].BaseShaderRegister = 0;
	descTblRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//b0
	descTblRange[1].NumDescriptors = 1;
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descTblRange[1].BaseShaderRegister = 0;
	descTblRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//デスクリプターテーブル設定
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam.DescriptorTable.NumDescriptorRanges = 2;
	rootParam.DescriptorTable.pDescriptorRanges = descTblRange;
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_DESC rsd = {};
	rsd.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rsd.NumParameters = 1;
	rsd.pParameters = &rootParam;

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 0.0f;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	samplerDesc.ShaderRegister = 0.0f;
	samplerDesc.RegisterSpace = 0.0f;

	rsd.pStaticSamplers = &samplerDesc;
	rsd.NumStaticSamplers = 1;

	result = D3D12SerializeRootSignature(&rsd, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	//ルートシグネチャの生成
	result = dev->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayoutDescs[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0,	DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//パイプラインステートオブジェクト(PSO)を作る
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};

	//ルートシグネチャと頂点レイアウト
	gpsDesc.pRootSignature = rootSignature;
	gpsDesc.InputLayout.pInputElementDescs = inputLayoutDescs;
	gpsDesc.InputLayout.NumElements = _countof(inputLayoutDescs);

	//頂点シェーダ
	gpsDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob);

	//ラスタライザ
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	//ピクセルシェーダ
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob);

	//使わない
	//gpsDesc.HS;
	//gpsDesc.DS;
	//gpsDesc.GS;

	//レンダーターゲット
	gpsDesc.NumRenderTargets = 1;//注)このターゲット数と設定するフォーマット数は
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//一致させておく事

													   //深度ステンシル
	gpsDesc.DepthStencilState.DepthEnable = false;//あとで
	gpsDesc.DepthStencilState.StencilEnable = false;//あとで
	gpsDesc.DSVFormat;//あとで

					  //その他
	gpsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsDesc.NodeMask = 0;
	gpsDesc.SampleDesc.Count = 1;//いる
	gpsDesc.SampleDesc.Quality = 0;//いる
	gpsDesc.SampleMask = 0xffffffff;//全部１
									//gpsDesc.Flags;//デフォルトでOK
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//三角形
	result = dev->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(&pipeline));
}

void Dx12Wrapper::InitTexture() {

	TexMetadata metadata;
	ScratchImage image;
	auto result = DirectX::LoadFromWICFile(L"devil.jpg", WIC_FLAGS_NONE, &metadata, image);

	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	heapprop.CreationNodeMask = 1;
	heapprop.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resDesc.Width = metadata.width;
	resDesc.Height = metadata.height;
	resDesc.DepthOrArraySize = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 1;
	resDesc.MipLevels = metadata.mipLevels;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	//テクスチャ読み込み
	auto fp = fopen("texturesample24bit.bmp", "rb");
	BITMAPFILEHEADER bmpfileH = {};
	BITMAPFILEHEADER bmpinfoH = {};
	fread(&bmpfileH, sizeof(bmpfileH), 1, fp);
	fread(&bmpinfoH, sizeof(bmpinfoH), 1, fp);

	ID3D12Resource* texBuff = nullptr;

	result = dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);

	//メルセンヌ・ツイスタ
	std::random_device rd;
	std::mt19937_64 mt(rd());

	//void* dummy = nullptr;
	//texBuff->Map(0, nullptr, &dummy);

	/*std::vector<unsigned char> data(256 * 256 * 4);
	for(int j = 256 - 1; j >= 0; --j){
	for(int i = 0; i < 256 * 4; i += 4){
	unsigned char bgr[3];
	fread(bgr, sizeof(unsigned char), 3, fp);
	data[j * 256 * 4 + i] = bgr[2];
	data[j * 256 * 4 + i + 1] = bgr[1];
	data[j * 256 * 4 + i + 2] = bgr[0];
	data[j * 256 * 4 + i + 3] = 255;
	}
	}*/

	/*for(int i = 0; i < data.size(); i += 4){
	data[i] = mt() % 256;
	data[i + 1] = mt() % 256;
	data[i + 2] = mt() % 256;
	data[i + 3] = 255;
	}*/


	fclose(fp);

	D3D12_BOX box;
	box.top = 0;
	box.left = 0;
	box.right = 256;
	box.bottom = 256;
	box.front = 0;
	box.back = 1;

	commandAllocator->Reset();
	commandList->Reset(commandAllocator, nullptr);

	result = texBuff->WriteToSubresource(0, nullptr, image.GetPixels(), metadata.width * 4, image.GetPixelsSize());

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(texBuff, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	commandList->Close();
	ExecuteCommand();
	WaitExecute();

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NodeMask = 0;
	descHeapDesc.NumDescriptors = 2;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&rgstDescHeap));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	dev->CreateShaderResourceView(texBuff, &srvDesc, rgstDescHeap->GetCPUDescriptorHandleForHeapStart());
}

void Dx12Wrapper::InitConstants() {
	Application& app = Application::Instance();
	auto wsize = app.GetWindowSize();

	XMMATRIX matrix = XMMatrixIdentity();

	XMFLOAT3 eye(10.0f, 0.0f, -20.0f);
	XMFLOAT3 target(0.0f, 0.0f, 0.0f);
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	auto camera = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	auto projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, static_cast<float>(wsize.w) / static_cast<float>(wsize.h), 0.1f, 300.0f);

	matrix *= camera;
	matrix *= projection;

	/*matrix.r[0].m128_f32[0] = 2.0f / static_cast<float>(wsize.w);
	matrix.r[1].m128_f32[1] = -2.0f / static_cast<float>(wsize.h);
	matrix.r[3].m128_f32[0] = -1.0f;
	matrix.r[3].m128_f32[1] = 1.0f;*/

	size_t size = sizeof(matrix);
	size = (size + 0xff) &~0xff;

	auto result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(matrix)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&cbuff)
	);

	XMMATRIX* m = nullptr;
	result = cbuff->Map(0, nullptr, (void**)&m);
	*m = matrix;

	D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
	desc.BufferLocation = cbuff->GetGPUVirtualAddress();
	desc.SizeInBytes = size;
	auto handle = rgstDescHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev->CreateConstantBufferView(&desc, handle);
}


Dx12Wrapper::Dx12Wrapper(HINSTANCE h, HWND hwnd) {
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	std::vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* tmpAdapter = nullptr;

	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

	for (int i = 0; dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters) {
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);
		std::wstring strDesc = adesc.Description;
		if (strDesc.find(L"NVIDIA") != std::string::npos) {//NVIDIAアダプタを強制
			tmpAdapter = adpt;
			break;
		}
	}

	//Direct3Dデバイスの初期化
	D3D_FEATURE_LEVEL featureLevel;
	for (auto l : levels) {
		D3D12CreateDevice(tmpAdapter, l, IID_PPV_ARGS(&dev));
		if (D3D12CreateDevice(nullptr, l, IID_PPV_ARGS(&dev)) == S_OK) {
			featureLevel = l;
			break;
		}
	}

	//コマンドキューオブジェクトの生成
	D3D12_COMMAND_QUEUE_DESC commandQDesc = {};
	commandQDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQDesc.NodeMask = 0;
	commandQDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	result = dev->CreateCommandQueue(&commandQDesc, IID_PPV_ARGS(&commandQueue));

	//スワップチェーンの生成
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	auto wsize = Application::Instance().GetWindowSize();
	swapchainDesc.Width = wsize.w;
	swapchainDesc.Height = wsize.h;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	result = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue,
		Application::Instance().GetWindowHandle(),
		&swapchainDesc,
		nullptr, nullptr,
		(IDXGISwapChain1**)(&swapchain)
	);

	//ビューポート設定
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = wsize.w;
	viewport.Height = wsize.h;
	viewport.MaxDepth = 1.0f;//カメラからの距離(遠いほう) 
	viewport.MinDepth = 0.0f;//カメラからの距離(近いほう) 

							 //なんでかシザー(切り取り)矩形も必要 
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = wsize.w;
	scissorRect.bottom = wsize.h;

	//デスクリプタを作るためのヒープ領域を確保
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descHeapDesc.NodeMask = 0;
	descHeapDesc.NumDescriptors = 2;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&rtvDescHeap));

	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpuDescH = rtvDescHeap->GetCPUDescriptorHandleForHeapStart();
		//各デスクリプタの使用するサイズを計算しておく
		auto rtvSize = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		DXGI_SWAP_CHAIN_DESC swchDesc = {};
		swapchain->GetDesc(&swchDesc);
		backBuffers.resize(swchDesc.BufferCount);
		for (int i = 0; i < backBuffers.size(); ++i) {
			swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
			dev->CreateRenderTargetView(backBuffers[i], nullptr, cpuDescH);
			cpuDescH.ptr += rtvSize;
		}
	}

	result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	result = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));

	dev->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	std::vector<unsigned short> indices = { 0,1,2,1,3,2 };

	ID3D12Resource* indexBuff = nullptr;
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(indices.size() * sizeof(indices[0])),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);

	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();//バッファの場所
	ibView.Format = DXGI_FORMAT_R16_UINT;//フォーマット(shortだからR16)
	ibView.SizeInBytes = indices.size() * sizeof(indices[0]);//総サイズ

	unsigned short* mappedIndexAdr = nullptr;
	indexBuff->Map(0, nullptr, (void**)&mappedIndexAdr);
	std::copy(indices.begin(), indices.end(), mappedIndexAdr);
	indexBuff->Unmap(0, nullptr);

	InitVertices();
	InitShader();
	InitTexture();
	InitConstants();
}

Dx12Wrapper::~Dx12Wrapper() {
	dev->Release();
	dxgiFactory->Release();
	swapchain->Release();
	commandQueue->Release();
	rtvDescHeap->Release();
	commandAllocator->Release();
	commandList->Release();

	//image.Release();
}

void Dx12Wrapper::Update() {
	commandAllocator->Reset();//アロケータリセット
	commandList->Reset(commandAllocator, pipeline);//コマンドリストリセット

	commandList->SetPipelineState(pipeline);
	commandList->SetGraphicsRootSignature(rootSignature);

	//ビューポートとシザー設定 
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	auto bbIdx = swapchain->GetCurrentBackBufferIndex();

	//レンダーダーゲット指定
	auto rtv = rtvDescHeap->GetCPUDescriptorHandleForHeapStart();
	rtv.ptr += bbIdx * dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	commandList->OMSetRenderTargets(1, &rtv, false, nullptr);
	float clearColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };//クリアカラー設定

	commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);//クリア

	commandList->SetDescriptorHeaps(1, &rgstDescHeap);
	commandList->SetGraphicsRootDescriptorTable(0, rgstDescHeap->GetGPUDescriptorHandleForHeapStart());

	commandList->IASetVertexBuffers(0, 1, &vbView);
	commandList->IASetIndexBuffer(&ibView);
	commandList->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->DrawIndexedInstanced(20, 1, 0, 0, 0);//描画
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIdx], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	commandList->Close();//コマンドのクローズ


	 //フェンスコマンド
	ExecuteCommand();
	//フェンスコマンド待ち
	WaitExecute();

	swapchain->Present(bbIdx, 0);
}

void Dx12Wrapper::ExecuteCommand() {
	ID3D12CommandList* cmdlists[] = { commandList };

	commandQueue->ExecuteCommandLists(1, cmdlists);
	commandQueue->Signal(fence, ++fenceValue);
}

void Dx12Wrapper::WaitExecute() {
	while (fence->GetCompletedValue() < fenceValue)
		;
}