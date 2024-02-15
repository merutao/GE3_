#pragma once
#include "DirectXCommon.h"
#include "SpriteCommon.h"
#include <d3d12.h>
#include <wrl.h>

#include <DirectXMath.h>

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
	struct MaterialData{
		Vector4 color;
		Matrix4x4 uvTransform;
	};

public:
	// ������
	void Initialize(SpriteCommon* common, std::wstring textureFilePath);

	//�X�V����
	void Update();

	//�`�揈��
	void Draw();

	//Getter/Setter
	Vector2 GetPosition() { return position; }
	float GetRotation() { return rotation; }
	Vector4 GetColor() { return color_; }
	Vector2 GetSize() { return size_; }
	//�A���J�[�|�C���g
	Vector2 GetAnchorPoint() { return anchorPoint; }
	//���E���]
	bool GetFlipX() { return isFlipX; }
	//�㉺���]
	bool GetFlipY() { return isFlipY; }

	//�؂蔲���֘A
	Vector2 GetTextureLeftTop() { return textureLeftTop; }
	Vector2 GetTextureSize() { return textureSize; }

	void SetPosition(Vector2 pos) { position = pos; }
	void SetRotation(float rot) { rotation = rot; }
	void SetColor(Vector4 color) { color_ = color; }
	void SetSize(Vector2 size) { this->size_ = size; }
	//�A���J�[�|�C���g
	void SetAnchorPoint(Vector2 anchor) { anchorPoint = anchor; }
	//���E���]
	void SetFlipX(bool isFlag) { isFlipX = isFlag; }
	//�㉺���]
	void SetFlipY(bool isFlag) { isFlipY = isFlag; }

	//�؂蔲���֘A
	void SetTextureLeftTop(Vector2 value) { textureLeftTop = value; }
	void SetTextureSize(Vector2 size) { textureSize = size; }

	void SetTexture(std::wstring textureFilePath);

private:
	//���_���쐬
	void CreateVertex();
	//�C���f�b�N�X���쐬
	void CreateIndex();
	//�}�e���A���쐬
	void CreateMaterial();
	//�s����쐬
	void CreateWVP();

	//�{���̉摜�̃T�C�Y�ŕ`�悷��(textureSize��M�炸�Ƃ��S�̑���`�悷��
	void AdjustTextureSize();

private:
	DirectXCommon* dxCommon_ = nullptr;
	SpriteCommon* common_ = nullptr;

	//���_���
	ComPtr<ID3D12Resource> vertexResource;
	//���_�o�b�t�@�r���[���쐬����
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//���_���\�[�X�Ƀf�[�^����������
	VertexData* vertexData = nullptr;

	//�C���f�b�N�X
	ComPtr<ID3D12Resource> indexResource;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	//�}�e���A�����
	ComPtr<ID3D12Resource> materialResource;
	MaterialData* materialData = nullptr;

	//�s����
	ComPtr<ID3D12Resource> wvpResource;
	Matrix4x4* wvpData = nullptr;

	//�p�����[�^
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	//UV���W
	Transform uvTransform = { {1,1,1},{0,0,0},{0,0,0} };

	//���@
	Transform transform = { {1,1,1},{0,0,0},{0,0,0} }; // Scale Rotate Translate
	Vector2 position = { 0.0f,0.0f };
	float rotation = 0;
	Vector2 size_ = { 512.0f,512.0f };

	//�A���J�[�|�C���g
	Vector2 anchorPoint = { 0,0 };
	//���E���]
	bool isFlipX = false;
	//�㉺���]
	bool isFlipY = false;

	//�؂蔲��
	//�؂蔲�������摜���̍��W
	Vector2 textureLeftTop = { 0,0 };
	//�؂蔲�������摜���̃T�C�Y
	Vector2 textureSize = { 0,0 };

	//�摜�̕ۑ�����Ă���ꏊ
	uint32_t textureIndex_ = 0;

	//�J����
	Transform cameraTransform = { {1,1,1},{0,0,0},{0,0,-5} };
};

