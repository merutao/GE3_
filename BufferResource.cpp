#include "BufferResource.h"

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
{
	//VertexResourceを生成する

	ID3D12Resource* resource = nullptr;

	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; //uploadHeapを使う
	//頂点リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes; //リソースのサイズ。
	//バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//実際に頂点リソースを作る
	HRESULT result;
	result = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(result));

	return resource;
}

ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
	//1.metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width); //Textureの幅
	resourceDesc.Height = UINT(metadata.height); //Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels); //mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); //奥行き　or　配列Tetureの配列数
	resourceDesc.Format = metadata.format; //TextureのFormat
	resourceDesc.SampleDesc.Count = 1; //サンプリングカウント。　1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); //Textureの次元数。普段使っているのは2次元

	//2.利用するHeapの設定
	// 非常に特殊な運用。"02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapPorperties{};
	heapPorperties.Type = D3D12_HEAP_TYPE_CUSTOM; //細かい設定を行う
	heapPorperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; //writeBackポリシーでCPUアクセス可能
	heapPorperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; //プロセッサの近くに配置

	//3.Resourceを生成する
	ID3D12Resource* resource = nullptr;
	HRESULT result;
	result = device->CreateCommittedResource(
		&heapPorperties, //Heapの設定
		D3D12_HEAP_FLAG_NONE, //Heapの特殊な設定。　特になし
		&resourceDesc, //Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ, //初回のResourceState。Textureは基本読むだけ
		nullptr, //Clear最適地。使わないのでnullptr
		IID_PPV_ARGS(&resource)  //作成するRedourceポインタへのポインタ
	);
	assert(SUCCEEDED(result));

	return resource;
}
