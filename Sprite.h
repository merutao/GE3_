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
	struct MaterialData{
		Vector4 color;
		Matrix4x4 uvTransform;
	};

public:
	// 初期化
	void Initialize(SpriteCommon* common, std::wstring textureFilePath);

	//更新処理
	void Update();

	//描画処理
	void Draw();

	//Getter/Setter
	Vector2 GetPosition() { return position; }
	float GetRotation() { return rotation; }
	Vector4 GetColor() { return color_; }
	Vector2 GetSize() { return size_; }
	//アンカーポイント
	Vector2 GetAnchorPoint() { return anchorPoint; }
	//左右反転
	bool GetFlipX() { return isFlipX; }
	//上下反転
	bool GetFlipY() { return isFlipY; }

	//切り抜き関連
	Vector2 GetTextureLeftTop() { return textureLeftTop; }
	Vector2 GetTextureSize() { return textureSize; }

	void SetPosition(Vector2 pos) { position = pos; }
	void SetRotation(float rot) { rotation = rot; }
	void SetColor(Vector4 color) { color_ = color; }
	void SetSize(Vector2 size) { this->size_ = size; }
	//アンカーポイント
	void SetAnchorPoint(Vector2 anchor) { anchorPoint = anchor; }
	//左右反転
	void SetFlipX(bool isFlag) { isFlipX = isFlag; }
	//上下反転
	void SetFlipY(bool isFlag) { isFlipY = isFlag; }

	//切り抜き関連
	void SetTextureLeftTop(Vector2 value) { textureLeftTop = value; }
	void SetTextureSize(Vector2 size) { textureSize = size; }

	void SetTexture(std::wstring textureFilePath);

private:
	//頂点情報作成
	void CreateVertex();
	//インデックス情報作成
	void CreateIndex();
	//マテリアル作成
	void CreateMaterial();
	//行列情報作成
	void CreateWVP();

	//本来の画像のサイズで描画する(textureSizeを弄らずとも全体像を描画する
	void AdjustTextureSize();

private:
	DirectXCommon* dxCommon_ = nullptr;
	SpriteCommon* common_ = nullptr;

	//頂点情報
	ComPtr<ID3D12Resource> vertexResource;
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;

	//インデックス
	ComPtr<ID3D12Resource> indexResource;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	//マテリアル情報
	ComPtr<ID3D12Resource> materialResource;
	MaterialData* materialData = nullptr;

	//行列情報
	ComPtr<ID3D12Resource> wvpResource;
	Matrix4x4* wvpData = nullptr;

	//パラメータ
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	//UV座標
	Transform uvTransform = { {1,1,1},{0,0,0},{0,0,0} };

	//自機
	Transform transform = { {1,1,1},{0,0,0},{0,0,0} }; // Scale Rotate Translate
	Vector2 position = { 0.0f,0.0f };
	float rotation = 0;
	Vector2 size_ = { 512.0f,512.0f };

	//アンカーポイント
	Vector2 anchorPoint = { 0,0 };
	//左右反転
	bool isFlipX = false;
	//上下反転
	bool isFlipY = false;

	//切り抜き
	//切り抜きたい画像内の座標
	Vector2 textureLeftTop = { 0,0 };
	//切り抜きたい画像内のサイズ
	Vector2 textureSize = { 0,0 };

	//画像の保存されている場所
	uint32_t textureIndex_ = 0;

	//カメラ
	Transform cameraTransform = { {1,1,1},{0,0,0},{0,0,-5} };
};

