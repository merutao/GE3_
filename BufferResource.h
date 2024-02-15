#pragma once
#include <d3d12.h>
#include <cassert>
#include <DirectXTex.h>

//Resource作成
ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);
//テクスチャリソースを作成する関数
ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);