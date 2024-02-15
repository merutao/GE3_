#include "TextureManager.h"
#include"BufferResource.h"

TextureManager* TextureManager::instance = nullptr;
uint32_t TextureManager::kSRVIndexTop = 1;

TextureManager* TextureManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new TextureManager();
	}

	return instance;
}

void TextureManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

void TextureManager::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;
	textureDatas.reserve(DirectXCommon::kMaxSRVCount);
}

void TextureManager::LoadTexture(const std::wstring& filePath)
{
	//�ǂݍ��ݍς݂��m�F���鏈��
	auto it = std::find_if(
		textureDatas.begin(),
		textureDatas.end(),
		[&](TextureData& textureData) {return textureData.filePath == filePath; }
	);
	if (it != textureDatas.end()) {
		//�ǂݍ��ݍς݂Ȃ瑁��return
		return;
	}

	//�ő吔�𒴂��Ȃ����`�F�b�N����
	assert(textureDatas.size() + kSRVIndexTop < DirectXCommon::kMaxSRVCount);

	//�e�N�X�`���t�@�C����ǂ�Ńv���O�����ň�����悤�ɂ���
	DirectX::ScratchImage image{};
	HRESULT result = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(result));

	//�~�b�v�}�b�v�̍쐬
	DirectX::ScratchImage mipImages{};
	result = DirectX::GenerateMipMaps(
		image.GetImages(), image.GetImageCount(), image.GetMetadata(),
		DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(result));

	//�e�N�X�`���f�[�^��ǉ� �摜�f�[�^�̒ǉ�
	textureDatas.resize(textureDatas.size() + 1);
	//�ǉ������e�N�X�`���f�[�^�̎Q�Ƃ��擾����
	TextureData& textureData = textureDatas.back();

	textureData.filePath = filePath;
	textureData.metaData = mipImages.GetMetadata();
	textureData.resource = CreateTextureResource(dxCommon_->GetDevice(), textureData.metaData);
	UploadTextureData(textureData.resource.Get(), mipImages);

	//�摜���ۑ�����Ă��郁����
	uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1) + kSRVIndexTop;
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = dxCommon_->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = dxCommon_->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	handleCPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * srvIndex;
	handleGPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * srvIndex;

	textureData.sevHandleCPU = handleCPU;
	textureData.srvHandleGPU = handleGPU;

	//ShaderResourceView�쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureData.metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = UINT(textureData.metaData.mipLevels);

	//SRV�̐���
	//�ǂݍ��񂾏���SrvDesc(�g)��Handle(�ʒu)���g���ĕۑ�����
	dxCommon_->GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.sevHandleCPU);
}

uint32_t TextureManager::GetTextureIndexFilePath(const std::wstring& filePath)
{
	auto it = std::find_if(
		textureDatas.begin(),
		textureDatas.end(),
		[&](TextureData& textureData) {return textureData.filePath == filePath; }
	);
	if (it != textureDatas.end()) {
		uint32_t textureIndex = static_cast<uint32_t>(std::distance(textureDatas.begin(), it));
		return textureIndex;
	}

	//�Ώۂ̉摜���Ȃ������ꍇ�A��~����
	assert(0);
	return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(uint32_t textureIndex)
{
	//�Ώۂ̗v�f�ԍ����������͈̔͊O��I�����Ă��Ȃ����m�F
	assert(textureIndex < DirectXCommon::kMaxSRVCount);

	//�v�f�ԍ���TextureData���󂯎��
	TextureData& data = textureDatas[textureIndex];

	return data.srvHandleGPU;
}

void TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
{
	//Mata�����擾
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//�SMipMap�ɂ���
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		//MipMapLevel���w�肵�ĊeImage���擾
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		//Texture�ɓ]��
		HRESULT result = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr, //�S�̈�փR�s�[
			img->pixels, //���f�[�^�A�h���X
			UINT(img->rowPitch), //1���C���T�C�Y
			UINT(img->slicePitch) //1���T�C�Y
		);
		assert(SUCCEEDED(result));
	}
}