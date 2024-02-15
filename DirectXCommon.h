#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include <vector>
#include <chrono>

#include "WinApp.h"

class DirectXCommon
{
private:
	//namespace�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // �����o�֐�
	// ������
	void Initialize(WinApp* winApp);

	//�`��O����
	void PreDraw();
	//�`��㏈��
	void PostDraw();

	//Getter
	ID3D12Device* GetDevice() const { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }

	//�X���b�v�`�F�[��
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc; }
	//RTV�f�B�X�N
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() { return rtvDesc; }
	//SRV�f�B�X�N���v�^�q�[�v
	ID3D12DescriptorHeap* GetSrvDescriptorHeap() { return srvDescriptorHeap.Get(); }

private:
	//�f�o�C�X
	void DeviceInitialize();
	//�R�}���h
	void CommandInitialize();
	//�X���b�v�`�F�[��
	void SwapChainInitialize();
	//�����_�[�^�[�Q�b�g
	void RenderTargetInitialize();
	//�[�x�o�b�t�@
	void DepthBufferInitialize();
	//�t�F���X
	void FenceInitialize();

	//�f�B�X�N���v�^�q�[�v
	ID3D12DescriptorHeap* CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);


	//FPS�Œ菉��������
	void InitializeFixFPS();
	//FPS�Œ�X�V����
	void UpdataFixFPS();

private:
	WinApp* winApp_ = nullptr;

	//�f�o�C�X
	ComPtr<ID3D12Device> device;
	ComPtr<IDXGIFactory7> dxgiFactory;
	//�R�}���h
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12CommandQueue> commandQueue;
	//�X���b�v�`�F�[��
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	ComPtr<IDXGISwapChain4> swapChain;
	//�����_�[�^�[�Q�b�g
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	// �o�b�N�o�b�t�@
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	//�[�x�o�b�t�@
	ComPtr<ID3D12Resource> depthBuff;
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	//�t�F���X
	ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal = 0;
	//�`��O����
	D3D12_RESOURCE_BARRIER barrierDesc{};

	// �����_�[�^�[�Q�b�g�r���[�̐ݒ�
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	//�L�^�p���Ԍv���̕ϐ�
	std::chrono::steady_clock::time_point reference_;

	//�f�B�X�N���v�^�q�[�v
	//RTV (�Q�[����ʂ�ۑ����Ă���)
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	//SRV (�摜�Ȃǂ�ۑ����Ă�������)
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
};