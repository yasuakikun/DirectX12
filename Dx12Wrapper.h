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
	XMMATRIX world;	//�����O���[���h
	XMMATRIX wvp;	//WVP�����ς�
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

	//�f�o�C�X
	Microsoft::WRL::ComPtr<ID3D12Device> dev;

	//�����_�[�^�[�Q�b�g�r���[�̂��߂̃q�[�v
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescHeap;

	//�e�N�X�`���A�萔�̂��߂̃q�[�v
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rgstDescHeap;

	//�f�v�X�X�e���V���r���[�̃q�[�v
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescHeap;

	//�e�N�X�`��
	D3D12_SHADER_RESOURCE_VIEW_DESC texView;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;	//�ŐV 6
	IDXGISwapChain4 * swapchain;	//�ŐV 4
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 fenceValue;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;//���ꂪ�ŏI�I�ɗ~�����I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3DBlob> signature;//���[�g�V�O�l�`�������邽�߂̍ޗ�

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;

	std::vector< Microsoft::WRL::ComPtr<ID3D12Resource> > backBuffers;

	Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;

	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW ibView;

	//���_�����`���A���_�o�b�t�@�����
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuff;

	//�R���X�^���g�o�b�t�@�p
	Microsoft::WRL::ComPtr<ID3D12Resource> cbuff;

	Microsoft::WRL::ComPtr<ID3D12Resource> dsvBuff;

	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuff;

	//�������֌W

	//�R�}���h�n������
	void InitCommandSet();

	//�����_�[�^�[�Q�b�g�r���[�̂��߂̃f�X�N���v�^�q�[�v���쐬
	void InitDescriptorHeapForRTV();

	//�X���b�v�`�F�[��������
	void InitRenderTargetViewFromSwapchain();

	//�t�F���X�̐���
	void InitFence();

	//�f�v�X�X�e���V���r���[�̏�����
	void InitBufferforDSV();

	//�f�v�X�X�e���V���r���[�̐���
	void InitDepthView();

	//���_�o�b�t�@�̏�����(�C���f�b�N�X�o�b�t�@��)
	void InitVertices();

	//�V�F�[�_�̏�����
	void InitShader();

	//���W�X�^�n�̏�����
	void InitDescriptorHeapForRegister();

	//���[�g�V�O�l�`���̏�����
	void InitRootSignature();

	//�p�C�v���C���X�e�[�g������
	void InitPipelineState();

	//�e�N�X�`���̏�����
	void InitTexture();

	//�R���X�^���g�o�b�t�@�̏�������
	//���e�N�X�`���̏������̌�ɌĂяo��
	void InitConstants();

	//�}�e���A���̏�����
	void InitMaterial();

	//�t�F���X
	void ExecuteCommand();
	void WaitExecute();

public:
	Dx12Wrapper(HINSTANCE h, HWND hwnd);
	~Dx12Wrapper();

	void Update();
};
