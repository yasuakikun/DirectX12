#include "PMDModel.h"
#include "Dx12Wrapper.h"
#include "Application.h"
#include <vector>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <d3dcompiler.h>
#include <tchar.h>

#include "d3dx12.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"DirectXTex.lib")

using namespace DirectX;

//コマンド系初期化
void Dx12Wrapper::InitCommandSet()
{
	D3D12_COMMAND_QUEUE_DESC commandQDesc = {};
	commandQDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQDesc.NodeMask = 0;
	commandQDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	auto result = dev->CreateCommandQueue(&commandQDesc, IID_PPV_ARGS(&commandQueue));

	result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	result = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
}

//レンダーターゲットビューのためのデスクリプタヒープを作成
void Dx12Wrapper::InitDescriptorHeapForRTV()
{
	//デスクリプタを作るためのヒープ領域を確保
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descHeapDesc.NodeMask = 0;
	descHeapDesc.NumDescriptors = 2;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	auto result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&rtvDescHeap));
}

//スワップチェーン初期化
void Dx12Wrapper::InitRenderTargetViewFromSwapchain()
{
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

	auto result = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue.Get(),
		Application::Instance().GetWindowHandle(),
		&swapchainDesc,
		nullptr, nullptr,
		(IDXGISwapChain1**)(&swapchain)
	);

	//各デスクリプタの使用するサイズを計算
	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescH = rtvDescHeap->GetCPUDescriptorHandleForHeapStart();

	auto rtvSize = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	DXGI_SWAP_CHAIN_DESC swchDesc = {};
	swapchain->GetDesc(&swchDesc);
	backBuffers.resize(swchDesc.BufferCount);
	for (int i = 0; i < backBuffers.size(); ++i)
	{
		swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		dev->CreateRenderTargetView(backBuffers[i].Get(), nullptr, cpuDescH);
		cpuDescH.ptr += rtvSize;
	}
}

//フェンスの生成
void Dx12Wrapper::InitFence()
{
	dev->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
}

//デプスステンシルビューの初期化
void Dx12Wrapper::InitBufferforDSV()
{
	auto wsize = Application::Instance().GetWindowSize();

	D3D12_RESOURCE_DESC desc = {};

	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.DepthOrArraySize = 1;
	desc.Width = wsize.w;
	desc.Height = wsize.h;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//このクリアバリューが重要な意味を持つので今回は作っておく
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;

	auto result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(&dsvBuff)
	);
}

//デプスステンシルビューの生成
void Dx12Wrapper::InitDepthView()
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};

	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	auto result = dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&dsvDescHeap));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	dev->CreateDepthStencilView(dsvBuff.Get(), &dsvDesc, dsvDescHeap->GetCPUDescriptorHandleForHeapStart());
}

//頂点バッファの初期化(インデックスバッファも)
void Dx12Wrapper::InitVertices()
{
	//頂点情報(Nの字の順)
	auto result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPUからGPUへ転送する用
		D3D12_HEAP_FLAG_NONE,//特別な指定なし
		&CD3DX12_RESOURCE_DESC::Buffer(model->GetModelData().size() * sizeof(PMDVerts)),//サイズ
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuff)
	);

	PMDVerts* vbuffptr = nullptr;
	result = vertexBuff->Map(0, nullptr, (void**)&vbuffptr);
	std::copy(model->GetModelData().begin(), model->GetModelData().end(), vbuffptr);
	vertexBuff->Unmap(0, nullptr);//mapをしたら確実にUnmapする

	vbView.BufferLocation = vertexBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = static_cast<unsigned int>(model->GetModelData().size()) * sizeof(PMDVerts);
	vbView.StrideInBytes = sizeof(PMDVerts);


	//インデックス情報
	ID3D12Resource* indexBuff = nullptr;
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(model->GetIndexData().size() * sizeof(model->GetIndexData()[0])),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);

	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();//バッファの場所
	ibView.Format = DXGI_FORMAT_R16_UINT;//フォーマット(shortだからR16)
	ibView.SizeInBytes = static_cast<unsigned int>(model->GetIndexData().size()) * sizeof(model->GetIndexData()[0]);//総サイズ

	unsigned short* mappedIndexAdr = nullptr;
	indexBuff->Map(0, nullptr, (void**)&mappedIndexAdr);
	std::copy(model->GetIndexData().begin(), model->GetIndexData().end(), mappedIndexAdr);
	indexBuff->Unmap(0, nullptr);
}

//シェーダの初期化
void Dx12Wrapper::InitShader()
{
	InitDescriptorHeapForRegister();

	InitRootSignature();

	InitPipelineState();
}

//レジスタの初期化
void Dx12Wrapper::InitDescriptorHeapForRegister()
{
	descTblRange.resize(3);

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

	//b0
	descTblRange[2].NumDescriptors = 1;
	descTblRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descTblRange[2].BaseShaderRegister = 1;
	descTblRange[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
}

//ルートシグネチャの初期化
void Dx12Wrapper::InitRootSignature()
{
	ID3DBlob * error = nullptr;

	CD3DX12_ROOT_PARAMETER rootParam = {};

	//デスクリプターテーブル設定
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam.DescriptorTable.NumDescriptorRanges = 3;
	rootParam.DescriptorTable.pDescriptorRanges = &descTblRange[0];
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
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	samplerDesc.ShaderRegister = 0;
	samplerDesc.RegisterSpace = 0;

	rsd.pStaticSamplers = &samplerDesc;
	rsd.NumStaticSamplers = 1;

	auto result = D3D12SerializeRootSignature(&rsd, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);

	//ルートシグネチャの生成
	result = dev->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
}

//パイプラインステート初期化
void Dx12Wrapper::InitPipelineState()
{
	ID3DBlob * vsBlob = nullptr;
	ID3DBlob * psBlob = nullptr;
	HRESULT result = S_OK;

	result = D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "vs", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vsBlob, nullptr);
	result = D3DCompileFromFile(L"shader.hlsl", nullptr, nullptr, "ps", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &psBlob, nullptr);

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayoutDescs[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//パイプラインステートオブジェクト(PSO)を作る
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};

	//ルートシグネチャと頂点レイアウト
	gpsDesc.pRootSignature = rootSignature.Get();
	gpsDesc.InputLayout.pInputElementDescs = inputLayoutDescs;
	gpsDesc.InputLayout.NumElements = _countof(inputLayoutDescs);

	//頂点シェーダ
	gpsDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob);

	//ピクセルシェーダ
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob);

	//使わない
	//gpsDesc.HS;
	//gpsDesc.DS;
	//gpsDesc.GS;

	//ラスタライザ
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	//レンダーターゲット
	gpsDesc.NumRenderTargets = 1;
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//一致させておく事

	//深度ステンシル
	gpsDesc.DepthStencilState.DepthEnable = true;
	gpsDesc.DepthStencilState.StencilEnable = false;
	gpsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpsDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;	//COMPAR = 比較, LESS = より小さい
	gpsDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

	//その他
	gpsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsDesc.NodeMask = 0;
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.SampleDesc.Quality = 0;
	gpsDesc.SampleMask = 0xffffffff;
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//三角形

	result = dev->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(&pipeline));
}

//テクスチャの初期化
void Dx12Wrapper::InitTexture()
{
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
	resDesc.Width = static_cast<unsigned int>(metadata.width);
	resDesc.Height = static_cast<unsigned int>(metadata.height);
	resDesc.DepthOrArraySize = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 1;
	resDesc.MipLevels = static_cast<unsigned int>(metadata.mipLevels);
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Resource* texBuff = nullptr;

	result = dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);

	commandAllocator->Reset();
	commandList->Reset(commandAllocator.Get(), nullptr);

	result = texBuff->WriteToSubresource(0, nullptr, image.GetPixels(), static_cast<unsigned int>(metadata.width) * 4, static_cast<unsigned int>(image.GetPixelsSize()));

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

//コンスタントバッファの初期化等
void Dx12Wrapper::InitConstants()
{
	auto wsize = Application::Instance().GetWindowSize();

	XMMATRIX matrix = XMMatrixIdentity();

	XMFLOAT3 eye(0.0f, 20.0f, -30.0f);	//視点
	XMFLOAT3 target(0.0f, 10.0f, 0.0f);	//注視点
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);		//上ベクトル

	//ストアは、保持

	camera = XMMatrixLookAtLH(
		XMLoadFloat3(&eye),
		XMLoadFloat3(&target),
		XMLoadFloat3(&up)
	);

	projection = XMMatrixPerspectiveFovLH(
		XM_PI / 3.0f,
		static_cast<float>(wsize.w) / static_cast<float>(wsize.h),
		0.1f,
		300.0f);

	matrix *= camera;		//W * V
	matrix *= projection;	//W * V * P

	size_t size = sizeof(matrix);
	size = (size + 0xff) &~ 0xff;

	dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(matrix)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&cbuff)
	);

	mappedmatrix = nullptr;
	auto result = cbuff->Map(0, nullptr, (void**)&mappedmatrix);
	*mappedmatrix = matrix;

	D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
	desc.BufferLocation = cbuff->GetGPUVirtualAddress();
	desc.SizeInBytes = static_cast<unsigned int>(size);
	auto handle = rgstDescHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev->CreateConstantBufferView(&desc, handle);
}

void Dx12Wrapper::InitMaterial()
{

	std::vector<ID3D12Resource*> matBuff;

	matBuff.resize(sizeof(Material));

	int midx = 0;
	size_t size = sizeof(Material);
	size = (size + 0xff) &~0xff;

	for (auto& mbuff : matBuff)
	{
		auto result = dev->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(size),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&materialBuff));

		/*Material* material = nullptr;
		result = mbuff->Map(0, nullptr, (void**)&material);
		*material = mat[midx];
		mbuff->Unmap(0, nullptr);
		++midx;*/
	}
}

Dx12Wrapper::Dx12Wrapper(HINSTANCE h, HWND hwnd)
{
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	//アダプターの選択
	std::vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* tmpAdapter = nullptr;

	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

	for(int i = 0; dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		adapters.push_back(tmpAdapter);
	}

	//NVIDIAを選択
	for(auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);
		std::wstring strDesc = adesc.Description;
		if(strDesc.find(L"NVIDIA") != std::string::npos)//NVIDIAアダプタを強制
		{
			tmpAdapter = adpt;
			break;
		}
	}

	//Direct3Dデバイスの初期化
	D3D_FEATURE_LEVEL featureLevel;
	for(auto l : levels)
	{
		D3D12CreateDevice(tmpAdapter, l, IID_PPV_ARGS(&dev));
		if(D3D12CreateDevice(nullptr, l, IID_PPV_ARGS(&dev)) == S_OK)
		{
			featureLevel = l;
			break;
		}
	}

	//モデルの読み込み(仮読み込み)
	model = std::make_shared<PMDModel>("Model/初音ミク.pmd");
	//model = std::make_shared<PMDModel>("Model/博麗霊夢/reimu_F01.pmd");

	//コマンド系初期化
	InitCommandSet();

	//レンダーターゲットビューのためのデスクリプタヒープを作成
	InitDescriptorHeapForRTV();

	//スワップチェーン初期化
	InitRenderTargetViewFromSwapchain();

	//フェンス初期化
	InitFence();

	//デプスステンシルビューの初期化
	InitBufferforDSV();

	//デプスステンシルビューの生成
	InitDepthView();

	//頂点バッファの初期化(インデックスバッファも)
	InitVertices();

	//シェーダの初期化
	InitShader();

	//テクスチャの初期化
	InitTexture();

	//コンスタントバッファの初期化等
	InitConstants();


	auto wsize = Application::Instance().GetWindowSize();

	//ビューポート設定
	viewport.TopLeftX = 0; 
	viewport.TopLeftY = 0; 
	viewport.Width = static_cast<float>(wsize.w);
	viewport.Height = static_cast<float>(wsize.h);
	viewport.MaxDepth = 1.0f;//カメラからの距離(遠いほう) 
	viewport.MinDepth = 0.0f;//カメラからの距離(近いほう) 
							 
	//なんでかシザー(切り取り)矩形も必要 
	scissorRect.left = 0; 
	scissorRect.top = 0; 
	scissorRect.right = wsize.w; 
	scissorRect.bottom = wsize.h;
}

Dx12Wrapper::~Dx12Wrapper()
{
	swapchain->Release();
}

void Dx12Wrapper::Update()
{
	commandAllocator->Reset();//アロケータリセット
	commandList->Reset(commandAllocator.Get(), pipeline.Get());//コマンドリストリセット

	commandList->SetPipelineState(pipeline.Get());
	commandList->SetGraphicsRootSignature(rootSignature.Get());

	//ビューポートとシザー設定 
	commandList->RSSetViewports(1, &viewport); 
	commandList->RSSetScissorRects(1, &scissorRect);
	auto bbIdx = swapchain->GetCurrentBackBufferIndex();

	//回転処理
	static float angle = 0.0f;
	XMMATRIX world = XMMatrixRotationY(angle);
	*mappedmatrix = world * camera * projection;
	angle += 0.03f;

	/*
	tm.world = XMMatrixRotationY(angle);
	tm.wvp = tm.world * XMMatrixLookAtLH(
							XMLoadFloat3(&eye),
							XMLoadFloat3(&target),
							XMLoadFloat3(&up)) * XMMatrixPerspectiveFovLH(XM_PIDIV2, static_cast<float>(wsize.w) / static_cast<float>(wsize.h),
							0.1f,
							300.0f);

	_constantBuffer.reset(_buffMgr->CreateConstantBuffer<TransformMatrices>("sample"));
	_constantBuffer->UpdateValue(tm);
	*/

	//レンダーダーゲット指定
	auto rtv = rtvDescHeap->GetCPUDescriptorHandleForHeapStart();
	rtv.ptr += bbIdx * dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//デプスステンシルビュー
	D3D12_CPU_DESCRIPTOR_HANDLE dsvs[] = { dsvDescHeap->GetCPUDescriptorHandleForHeapStart() };

	commandList->OMSetRenderTargets(1, &rtv, false, dsvs);
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };	//クリアカラー設定

	commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);	//クリア
	commandList->ClearDepthStencilView(dsvs[0], D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	
	ID3D12DescriptorHeap* descHeap = { rgstDescHeap.Get() };

	commandList->SetDescriptorHeaps(1, &descHeap);
	commandList->SetGraphicsRootDescriptorTable(0, rgstDescHeap->GetGPUDescriptorHandleForHeapStart());

	commandList->IASetVertexBuffers(0, 1, &vbView);
	commandList->IASetIndexBuffer(&ibView);

	commandList->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	unsigned int vert_start = 0;

	for (auto& mat : model->Materials())
	{
		auto vert_count = mat.face_vert_count;
		
		commandList->DrawIndexedInstanced(vert_count, 1, vert_start, 0, 0);//描画

		vert_start += vert_count;
	}

	//commandList->DrawIndexedInstanced(static_cast<unsigned int>(model->GetIndexData().size()), 1, 0, 0, 0);//描画

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIdx].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	commandList->Close();//コマンドのクローズ


	//フェンスコマンド
	ExecuteCommand();
	//フェンスコマンド待ち
	WaitExecute();

	swapchain->Present(bbIdx, 0);
}

void Dx12Wrapper::ExecuteCommand(){
	ID3D12CommandList* cmdlists[] = { commandList.Get() };

	commandQueue->ExecuteCommandLists(1, cmdlists);
	commandQueue->Signal(fence.Get(), ++fenceValue);
}

void Dx12Wrapper::WaitExecute(){
	while(fence->GetCompletedValue() < fenceValue)
		;
}
