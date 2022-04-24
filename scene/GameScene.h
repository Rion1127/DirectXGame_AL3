#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "DebugText.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <DirectXMath.h>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

  public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//パーツID
	enum PartId {
		Root,	//大本	0
		Spine,	//脊髄	1
		Chest,	//胸		2
		Head,	//頭		3
		ArmL,	//左腕	4
		ArmR,	//右腕	5
		Hip,	//尻		6
		LegL,	//左足	7
		LegR,	//右足	8
	};

  private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0;
	//スプライト
	Sprite* sprite_ = nullptr;
	//3Dモデル
	Model* model_ = nullptr;
	//ワールドトランスフォーム
	WorldTransform worldTransform_[100];
	//ビュープロジェクション
	ViewProjection viewProjection_;
	//サウンドハンドルデータ
	uint32_t soundDatahandle_ = 0;
	//音声再生ハンドル
	uint32_t voiceHandle_ = 0;
	//値を表示したい変数
	int32_t value_ = 0;
	//カメラ上方向の角度
	float viewAngle = 0.0f;
	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
