#pragma once
#include "DirectXCommon.h"
#include <d3d12.h>
#include <wrl.h>

#include <DirectXMath.h>

#include "SpriteCommon.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

// �X�v���C�g
class Sprite
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	struct  Transform
	{
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	//���_���
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
	};

	//�}�e���A��
	struct MaterialData {
		Vector4 color;
		Matrix4x4 uvTransform;
	};

public:
	// ������
	void Initialize(SpriteCommon* common);

	//�X�V����
	void Update();

	//�`�揈��
	void Draw();

private:
	//���_���쐬
	void CreateVertex();
	//�C���f�b�N�X���쐬
	void CreateIndex();
	//�}�e���A���쐬
	void CreateMaterial();
	//�s����쐬
	void CreateWVP();

private:
	DirectXCommon* dxCommon_ = nullptr;
	SpriteCommon* common_ = nullptr;

	//���_���
	ComPtr<ID3D12Resource> vertexResource;
	//���_�o�b�t�@�r���[���쐬����
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	//�C���f�b�N�X
	ComPtr<ID3D12Resource> indexResource;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	//�}�e���A�����
	ComPtr<ID3D12Resource> materialResource;
	MaterialData* materialData = nullptr;

	//�s����
	ComPtr<ID3D12Resource> wvpResource;
	Matrix4x4* wvpData = nullptr;

	//�摜�̕ۑ���̃A�h���X
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;

	//�p�����[�^
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	//UV���W
	Transform uvTransform = { {1,1,1},{0,0,0},{0,0,0} };

	//���@
	Transform transform = { {1,1,1},{0,0,0},{0,0,0} }; 

	//�J����
	Transform cameraTransform = { {1,1,1},{0,0,0},{0,0,-5} };
};
