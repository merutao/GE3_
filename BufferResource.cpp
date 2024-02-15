#include "BufferResource.h"

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
{
	//VertexResource�𐶐�����

	ID3D12Resource* resource = nullptr;

	//���_���\�[�X�p�̃q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; //uploadHeap���g��
	//���_���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC resourceDesc{};
	//�o�b�t�@���\�[�X�B�e�N�X�`���̏ꍇ�͂܂��ʂ̐ݒ������
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes; //���\�[�X�̃T�C�Y�B
	//�o�b�t�@�̏ꍇ�͂�����1�ɂ��錈�܂�
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	//�o�b�t�@�̏ꍇ�͂���ɂ��錈�܂�
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//���ۂɒ��_���\�[�X�����
	HRESULT result;
	result = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(result));

	return resource;
}

ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
	//1.metadata�����Resource�̐ݒ�
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width); //Texture�̕�
	resourceDesc.Height = UINT(metadata.height); //Texture�̍���
	resourceDesc.MipLevels = UINT16(metadata.mipLevels); //mipmap�̐�
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); //���s���@or�@�z��Teture�̔z��
	resourceDesc.Format = metadata.format; //Texture��Format
	resourceDesc.SampleDesc.Count = 1; //�T���v�����O�J�E���g�B�@1�Œ�
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); //Texture�̎������B���i�g���Ă���̂�2����

	//2.���p����Heap�̐ݒ�
	// ���ɓ���ȉ^�p�B"02_04ex�ň�ʓI�ȃP�[�X�ł�����
	D3D12_HEAP_PROPERTIES heapPorperties{};
	heapPorperties.Type = D3D12_HEAP_TYPE_CUSTOM; //�ׂ����ݒ���s��
	heapPorperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; //writeBack�|���V�[��CPU�A�N�Z�X�\
	heapPorperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; //�v���Z�b�T�̋߂��ɔz�u

	//3.Resource�𐶐�����
	ID3D12Resource* resource = nullptr;
	HRESULT result;
	result = device->CreateCommittedResource(
		&heapPorperties, //Heap�̐ݒ�
		D3D12_HEAP_FLAG_NONE, //Heap�̓���Ȑݒ�B�@���ɂȂ�
		&resourceDesc, //Resource�̐ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ, //�����ResourceState�BTexture�͊�{�ǂނ���
		nullptr, //Clear�œK�n�B�g��Ȃ��̂�nullptr
		IID_PPV_ARGS(&resource)  //�쐬����Redource�|�C���^�ւ̃|�C���^
	);
	assert(SUCCEEDED(result));

	return resource;
}
