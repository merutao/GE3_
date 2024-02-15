#include "Sprite.h"

#include <DirectXMath.h>

#include "BufferResource.h"
#include "gMath.h"
#include "Externals/imgui/imgui.h"

using namespace Microsoft::WRL;
using namespace DirectX;

void Sprite::Initialize(SpriteCommon* common)
{
	common_ = common;
	dxCommon_ = common_->GetDirectXCommon();

	//�摜�̓ǂݍ���
	DirectX::ScratchImage mipImages = common->LoadTexture(L"Resources/mario.jpg");
	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();
	ID3D12Resource* textureResource = CreateTextureResource(dxCommon_->GetDevice(), metaData);
	common_->UploadTextureData(textureResource, mipImages);

	//ShaderResourceView�쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = UINT(metaData.mipLevels);

	//�ۑ��������̏ꏊ���w��
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon_->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	textureSrvHandleGPU = dxCommon_->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	//�擪��ImGui���g���Ă���̂ł��̎����g��
	textureSrvHandleCPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//SRV�̐���
	//�ǂݍ��񂾏���SrvDesc(�g)��Handle(�ʒu)���g���ĕۑ�����
	dxCommon_->GetDevice()->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);

	//���_���
	CreateVertex();
	//�C���f�b�N�X���
	CreateIndex();
	//�F
	CreateMaterial();
	//�s��
	CreateWVP();
}

void Sprite::Update()
{
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
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, (float)WinApp::window_width / (float)WinApp::window_height, 0.1f, 100.0f);
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
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

	//���_���̂ݕ`��
	//dxCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);
	//�C���f�b�N�X��񂪂���ꍇ�̕`��
	dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
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

	//���_���\�[�X�Ƀf�[�^����������
	VertexData* vertexData = nullptr;
	//�������ނ��߂̃A�h���X���擾
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//����
	vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	//��
	vertexData[1].position = { -0.5f,0.5f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.0f,0.0f };
	//�E��
	vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };
	//��2
	vertexData[3].position = { 0.5f,0.5f,0.0f,1.0f };
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
