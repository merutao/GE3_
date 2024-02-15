#include "Sprite.h"

#include <DirectXMath.h>

#include "BufferResource.h"
#include "gMath.h"

#include "Externals/imgui/imgui.h"

#include "TextureManager.h"

using namespace Microsoft::WRL;
using namespace DirectX; 

void Sprite::Initialize(SpriteCommon* common, std::wstring textureFilePath)
{
	common_ = common;
	dxCommon_ = common_->GetDirectXCommon();

	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexFilePath(textureFilePath);

	//���_���
	CreateVertex();
	//�C���f�b�N�X���
	CreateIndex();
	//�F
	CreateMaterial();
	//�s��
	CreateWVP();

	//�摜�̃T�C�Y�𐮗�����
	AdjustTextureSize();
}

void Sprite::Update()
{
	//�X�V����
	transform.translate = { position.x,position.y,0.0f };
	transform.rotate = { 0.0f,0.0f,rotation };
	materialData->color = color_;
	transform.scale = { size_.x,size_.y,1.0f };

	//�A���J�[�|�C���g�X�V
	float left = 0.0f - anchorPoint.x;
	float right = 1.0f - anchorPoint.x;
	float top = 0.0f - anchorPoint.y;
	float bottom = 1.0f - anchorPoint.y;

	//�t���b�v
	if (isFlipX == true) {
		//���E���]
		left = -left;
		right = -right;
	}
	if (isFlipY == true) {
		//�㉺���]
		top = -top;
		bottom = -bottom;
	}

	//���_���
	//����
	vertexData[0].position = { left,bottom,0.0f,1.0f };
	//��						  
	vertexData[1].position = { left,top,0.0f,1.0f };
	//�E��
	vertexData[2].position = { right,bottom,0.0f,1.0f };
	//��2					   
	vertexData[3].position = { right,top,0.0f,1.0f };

	const DirectX::TexMetadata& metaData = TextureManager::GetInstance()->GetMetaData(textureIndex_);
	float tex_left = textureLeftTop.x / metaData.width;
	float tex_right = (textureLeftTop.x + textureSize.x) / metaData.width;
	float tex_top = textureLeftTop.y / metaData.height;
	float tex_bottom = (textureLeftTop.y + textureSize.y) / metaData.height;

	//UV���W
	vertexData[0].texcoord = { tex_left,tex_bottom };
	vertexData[1].texcoord = { tex_left,tex_top };
	vertexData[2].texcoord = { tex_right,tex_bottom };
	vertexData[3].texcoord = { tex_right,tex_top };

	ImGui::Begin("Texture");
	ImGui::DragFloat3("Pos", &transform.translate.x, 0.1f);

	ImGui::DragFloat3("UV-Pos", &uvTransform.translate.x, 0.01f, -10.f, 10.f);
	ImGui::SliderAngle("UV-Rot", &uvTransform.rotate.z);
	ImGui::DragFloat3("UV-Scale", &uvTransform.scale.x, 0.01f, -10.f, 10.f);

	ImGui::End();
}

void Sprite::Draw()
{
	//Y�����S�ɉ�]
	//transform.rotate.y += 0.03f;
	//���[���h
	//�s��ϊ�
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

	//�J����
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	//View
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	//Projection
	
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0, 0,(float)WinApp::window_width, (float)WinApp::window_height, 0.1f, 100.0f);
	
	//WVP
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	//�s��̑��
	*wvpData = worldViewProjectionMatrix;

	//UV���W
	Matrix4x4 uvWorldMatrix = MakeAffineMatrix(uvTransform.scale, uvTransform.rotate, uvTransform.translate);
	materialData->uvTransform = uvWorldMatrix;

	// �`��
	//���_���
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	//�C���f�b�N�X���
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView);

	//�F
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//�s��
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	//�摜
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));

	//���_���̂ݕ`��
	
    //�C���f�b�N�X��񂪂���ꍇ�̕`��
	dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::SetTexture(std::wstring textureFilePath)
{
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexFilePath(textureFilePath);
}

void Sprite::CreateVertex()
{
	//VertexResource�𐶐�����
	vertexResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * 4);

	//���\�[�X�̐擪�̃A�h���X����g��
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//�g�p���郊�\�[�X�̃T�C�Y
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
	//1���_������̃T�C�Y
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//�������ނ��߂̃A�h���X���擾
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//����
	vertexData[0].position = { 0.0f,1.0f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	//��
	vertexData[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.0f,0.0f };
	//�E��
	vertexData[2].position = { 1.0f,1.0f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };
	//��2
	vertexData[3].position = { 1.0f,0.0f,0.0f,1.0f };
	vertexData[3].texcoord = { 1.0f,0.0f };
}

void Sprite::CreateIndex()
{
	indexResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * 6);

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	//VertexData[0,1,2]�̒��_�ŎO�p�`���ꖇ�쐬
	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;

	//VertexData[1,3,2]�̒��_�ŎO�p�`���ꖇ�쐬
	indexData[3] = 1;
	indexData[4] = 3;
	indexData[5] = 2;
}

void Sprite::CreateMaterial()
{
	materialResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(MaterialData));

	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	materialData->color = color_;
	materialData->uvTransform = MakeIdentity4x4();

}

void Sprite::CreateWVP()
{
	wvpResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(XMMATRIX));

	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));

	*wvpData = MakeIdentity4x4();
}

void Sprite::AdjustTextureSize()
{
	const DirectX::TexMetadata& metaData = TextureManager::GetInstance()->GetMetaData(textureIndex_);

	textureSize.x = static_cast<float> (metaData.width);
	textureSize.y = static_cast<float> (metaData.height);

	size_ = textureSize;
}
