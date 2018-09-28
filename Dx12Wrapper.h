#pragma once
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>

class Dx12Wrapper
{
private:

	ID3D12Device * dev;

	//�����_�[�^�[�Q�b�g�r���[�̂��߂̃q�[�v
	ID3D12DescriptorHeap* rtvDescHeap;
	//�e�N�X�`���A�萔�̂��߂̃q�[�v
	ID3D12DescriptorHeap* rgstDescHeap;
	//�e�N�X�`��
	D3D12_SHADER_RESOURCE_VIEW_DESC texView;

	ID3D12CommandQueue* commandQueue;
	IDXGIFactory5* dxgiFactory;
	IDXGISwapChain4* swapchain;
	ID3D12CommandAllocator* commandAllocator;
	ID3D12GraphicsCommandList* commandList;
	ID3D12Fence* fence;
	UINT64 fenceValue;
	ID3D12PipelineState* pipeline;
	ID3D12RootSignature* rootSignature;//���ꂪ�ŏI�I�ɗ~�����I�u�W�F�N�g
	ID3DBlob* signature;//���[�g�V�O�l�`�������邽�߂̍ޗ�

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;

	std::vector<ID3D12Resource*> backBuffers;

	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;

	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW ibView;

	//���_�����`���A���_�o�b�t�@�����
	ID3D12Resource* vertexBuff;

	//�R���X�^���g�o�b�t�@�p
	ID3D12Resource* cbuff;

	void InitVertices();
	void InitShader();
	void InitTexture();

	//�R���X�^���g�o�b�t�@�̏�������
	//���e�N�X�`���̏������̌�ɌĂяo��
	void InitConstants();
public:
	Dx12Wrapper(HINSTANCE h, HWND hwnd);
	~Dx12Wrapper();
	void Update();
	void ExecuteCommand();
	void WaitExecute();
};

