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

// スプライト
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

	//頂点情報
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
	};

	//マテリアル
	struct MaterialData {
		Vector4 color;
		Matrix4x4 uvTransform;
	};

public:
	// 初期化
	void Initialize(SpriteCommon* common);

	//更新処理
	void Update();

	//描画処理
	void Draw();

private:
	//頂点情報作成
	void CreateVertex();
	//インデックス情報作成
	void CreateIndex();
	//マテリアル作成
	void CreateMaterial();
	//行列情報作成
	void CreateWVP();

private:
	DirectXCommon* dxCommon_ = nullptr;
	SpriteCommon* common_ = nullptr;

	//頂点情報
	ComPtr<ID3D12Resource> vertexResource;
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	//インデックス
	ComPtr<ID3D12Resource> indexResource;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	//マテリアル情報
	ComPtr<ID3D12Resource> materialResource;
	MaterialData* materialData = nullptr;

	//行列情報
	ComPtr<ID3D12Resource> wvpResource;
	Matrix4x4* wvpData = nullptr;

	//画像の保存先のアドレス
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;

	//パラメータ
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	//UV座標
	Transform uvTransform = { {1,1,1},{0,0,0},{0,0,0} };

	//自機
	Transform transform = { {1,1,1},{0,0,0},{0,0,0} }; 

	//カメラ
	Transform cameraTransform = { {1,1,1},{0,0,0},{0,0,-5} };
};
