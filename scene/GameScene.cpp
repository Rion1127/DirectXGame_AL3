﻿#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() {

	delete sprite_;
	delete model_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//乱数シード生成期
	std::random_device seed_gen;
	//メルセンヌ・ツイスター
	std::mt19937_64 engine(seed_gen());
	//乱数範囲(回転角用)
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	//乱数範囲（座標用）
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");
	//スプライトの生成
	sprite_ = Sprite::Create(textureHandle_, {100, 50});
	// 3Dモデルの生成
	model_ = Model::Create();

	// for (int i = 0; i < _countof(worldTransform_); i++) {
	//// X, Y, Z方向のスケーリングを設定
	// worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f};
	//// X, Y, Z軸周りの回転角を設定
	// worldTransform_[i].rotation_ = {rotDist(engine), rotDist(engine), rotDist(engine)};
	////度数法の角度で考えたい場合は、XMConvertToRadians(float)を入れる
	//// X, Y, Z軸周りの平行移動を設定
	// worldTransform_[i].translation_ = {posDist(engine), posDist(engine), posDist(engine)};

	//ワールドトランスフォームの初期化
	// worldTransform_[i].Initialize();
	//}

	//親（0番目）
	worldTransform_[PartId::Root].translation_ = {0, 0, 0};
	worldTransform_[PartId::Root].Initialize();
	//子（1番）
	worldTransform_[PartId::Spine].translation_ = {0, 4.5f, 0};
	worldTransform_[PartId::Spine].parent_ = &worldTransform_[PartId::Root];
	worldTransform_[PartId::Spine].Initialize();

//上半身
#pragma region
	worldTransform_[PartId::Chest].translation_ = {0, 0.0f, 0};
	worldTransform_[PartId::Chest].parent_ = &worldTransform_[PartId::Spine];
	worldTransform_[PartId::Chest].Initialize();

	worldTransform_[PartId::Head].translation_ = {0.0f, 0.0f, 3.0f};
	worldTransform_[PartId::Head].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::Head].Initialize();

	worldTransform_[PartId::ArmL].translation_ = {3.0f, 0.0f, 0};
	worldTransform_[PartId::ArmL].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::ArmL].Initialize();

	worldTransform_[PartId::ArmR].translation_ = {-3.0f, 0.0f, 0};
	worldTransform_[PartId::ArmR].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::ArmR].Initialize();
#pragma endregion
//下半身
#pragma region
	/*worldTransform_[PartId::Hip].translation_ = {0, -3.0f, 0};
	worldTransform_[PartId::Hip].parent_ = &worldTransform_[PartId::Spine];
	worldTransform_[PartId::Hip].Initialize();

	worldTransform_[PartId::LegL].translation_ = {3.0f, -3.0f, 0};
	worldTransform_[PartId::LegL].parent_ = &worldTransform_[PartId::Hip];
	worldTransform_[PartId::LegL].Initialize();

	worldTransform_[PartId::LegR].translation_ = {-3.0f, -3.0f, 0};
	worldTransform_[PartId::LegR].parent_ = &worldTransform_[PartId::Hip];
	worldTransform_[PartId::LegR].Initialize();*/

#pragma endregion

	worldTransform_[50].translation_ = {-10.0f, 4.0f, -10.0f};
	worldTransform_[50].Initialize();
	worldTransform_[51].translation_ = {10.0f, 4.0f, -10.0f};
	worldTransform_[51].Initialize();
	worldTransform_[52].translation_ = {-10.0f, 4.0f, 0};
	worldTransform_[52].Initialize();
	worldTransform_[53].translation_ = {10.0f, 4.0f, 0};
	worldTransform_[53].Initialize();

	//カメラ視点座標を設定
	viewProjection_.eye = {0, 30, -50};
	//カメラ注視点座標を設定
	viewProjection_.target = {0, 0, 0};
	//カメラ上方向ベクトルを設定（右上45度指定）
	viewProjection_.up = {/*cosf(XM_PI / 1.0f)*/ 0.0f, /*sinf(XM_PI / 1.0f)*/ 1.0f, 0.0f};

	//カメラ垂直方向視野角を設定
	viewProjection_.fovAngleY = XMConvertToRadians(45.0f);

	//ニアクリップ距離を設定
	// viewProjection_.nearZ = 52.0f;
	//ファークリップ距離を設定
	// viewProjection_.farZ = 53.0f;

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
	//サウンドデータの読み込み
	soundDatahandle_ = audio_->LoadWave("se_sad03.wav");
	//音声再生
	// voiceHandle_ = audio_->PlayWave(soundDatahandle_,true);
}

void GameScene::Update() {
	//視点の移動ベクトル
	XMFLOAT3 moveA = {0, 0, 0};
	XMFLOAT3 targetmove = {0, 0, 0};
	//視点の移動速さ
	const float kEyeSpeed = 0.2f;
	const float targetSpeed = 0.2f;

	////押した方向で移動ベクトルを変更

	// if (input_->PushKey(DIK_W)) {
	//	move = {0, 0, kEyeSpeed};
	// } else if (input_->PushKey(DIK_S)) {
	//	move = {0, 0, -kEyeSpeed};
	// }
	////押した方向でターゲットを変更
	// if (input_->PushKey(DIK_LEFT)) {
	//	targetmove = {-targetSpeed, 0, 0};
	// } else if (input_->PushKey(DIK_RIGHT)) {
	//	targetmove = {targetSpeed, 0, 0};
	// }

	////視点移動（ベクトルの加算）

	// viewProjection_.eye.x += move.x;
	// viewProjection_.eye.y += move.y;
	// viewProjection_.eye.z += move.z;

	////視点移動（ベクトルの加算）

	// viewProjection_.target.x += targetmove.x;
	// viewProjection_.target.y += targetmove.y;
	// viewProjection_.target.z += targetmove.z;

	//上方向の回転速さ[ラジアン/frame]
	const float kPuRotSpeed = 0.05f;

	////押した方向で移動ベクトルを変更

	// if (input_->PushKey(DIK_SPACE)) {
	//	viewAngle += kPuRotSpeed;
	//	// 2πを超えたら0に戻す
	//	viewAngle = fmodf(viewAngle, XM_2PI);
	// }
	////上方向ベクトルを計算（半径1の演習場の座標）
	// viewProjection_.up = {cosf(viewAngle), sinf(viewAngle), 0.0f};

	////上キーで視野角が広がる

	// if (input_->PushKey(DIK_W)) {
	//	viewProjection_.fovAngleY += 0.01f;
	//	viewProjection_.fovAngleY = min(viewProjection_.fovAngleY, XM_PI);
	// }

	////下キーで視野角が狭まる

	// else if (input_->PushKey(DIK_S)) {
	//	viewProjection_.fovAngleY -= 0.01f;
	//	viewProjection_.fovAngleY = max(viewProjection_.fovAngleY, 0.01f);
	// }

	//アスペクト比を設定
	// viewProjection_.aspectRatio = 1.0f;

	//上下キーでニアクリップ距離を増

	/*if (input_->PushKey(DIK_UP)) {
	    viewProjection_.nearZ += 0.1f;
	} else if (input_->PushKey(DIK_DOWN)) {
	    viewProjection_.nearZ -= 0.1f;
	}*/

	//////////////
	//行列の再計算//
	/////////////
	viewProjection_.UpdateMatrix();

////////////////////
//キャラクター移動処理//
////////////////////
#pragma region キャラクター移動
	//キャラクターの移動ベクトル
	XMFLOAT3 move = {0, 0, 0};
	//キャラクターの移動速さ
	float kCharacterSpeed = 0.2f;

	//上半身回転処理
	//上半身の回転の速さ[ラジアン/frame]
	const float kChestRotSpeed = 0.05f;

	XMFLOAT3 frontVec = {0, 0, 1};
	XMFLOAT3 resultVec = {0, 0, 0};
	float moveSopeed = 0.2f;

	//プレイヤーの正面ベクトル
	resultVec.x = {
	  cos(worldTransform_[PartId::Chest].rotation_.y) * frontVec.x
	  +sin(worldTransform_[PartId::Chest].rotation_.y) * frontVec.z
	};

	resultVec.z = {
	  -sin(worldTransform_[PartId::Chest].rotation_.y) * frontVec.x +
	  cos(worldTransform_[PartId::Chest].rotation_.y) * frontVec.z
	};

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_UP)) {
		move = {resultVec.x * moveSopeed, 0, resultVec.z * moveSopeed};
	} else if (input_->PushKey(DIK_DOWN)) {
		move = {-resultVec.x * moveSopeed, 0, -resultVec.z * moveSopeed};
	}

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_LEFT)) {
		worldTransform_[PartId::Chest].rotation_.y -= kChestRotSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		worldTransform_[PartId::Chest].rotation_.y += kChestRotSpeed;
	}

	//下半身回転処理
	//下半身の回転の速さ[ラジアン/frame]
	const float kHipRotSpeed = 0.05f;

	//押した方向で移動ベクトルを変更
	/*if (input_->PushKey(DIK_J)) {
	    worldTransform_[PartId::Hip].rotation_.y -= kHipRotSpeed;
	} else if (input_->PushKey(DIK_K)) {
	    worldTransform_[PartId::Hip].rotation_.y += kHipRotSpeed;
	}*/

	//大本から更新していく
	worldTransform_[PartId::Root].UpdateMatrix();
	worldTransform_[PartId::Spine].UpdateMatrix();
	for (int i = PartId::Chest; i <= PartId::ArmR; i++) {
		worldTransform_[i].UpdateMatrix();
	}

	//注視点移動
	worldTransform_[PartId::Root].translation_.x += move.x;
	worldTransform_[PartId::Root].translation_.y += move.y;
	worldTransform_[PartId::Root].translation_.z += move.z;

#pragma endregion

#pragma region カメラ処理

	XMFLOAT3 resultEye;
	XMFLOAT3 resultTarget;
	float cameraDistance = 50;
	//カメラ注視点座標指定
	resultTarget = worldTransform_[PartId::Root].translation_;

	resultEye.x = -resultVec.x * cameraDistance + resultTarget.x;
	resultEye.y = 30.0f;
	resultEye.z = -resultVec.z * cameraDistance + resultTarget.z;

	//カメラ視点座標を設定
	viewProjection_.eye = {resultEye.x, resultEye.y,resultEye.z};
	//カメラ注視点座標を設定
	viewProjection_.target = {resultTarget.x, resultTarget.y, resultTarget.z};

#pragma endregion

	//スプライトの今の座標を取得
	XMFLOAT2 position = sprite_->GetPosition();
	//座標を{ 2, 0}移動
	position.x += 2.0f;
	position.y += 1.0f;
	//移動した座標をスプライトに反映
	sprite_->SetPosition(position);

	//スペースキーを押した瞬間
	if (input_->TriggerKey(DIK_SPACE)) {
		//音声停止
		audio_->StopWave(voiceHandle_);
	}

	//デバッグテキストの表示
	debugText_->SetPos(50, 50);
	debugText_->SetPos(50, 50);
	debugText_->Printf("Chest:rotation_.y(%f)", worldTransform_[PartId::Chest].rotation_.y);
	debugText_->SetPos(50, 70);
	debugText_->Printf(
	  "Chest:rotation_.y sin(%f)", sin(worldTransform_[PartId::Chest].rotation_.y));
	debugText_->SetPos(50, 90);
	debugText_->Printf(
	  "Chest:rotation_.y cos(%f)", cos(worldTransform_[PartId::Chest].rotation_.y));
	debugText_->SetPos(50, 110);
	debugText_->Printf("resultEye.x,y,z cos(%f,%f,%f)", resultVec.x, resultVec.y, resultVec.z);
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	/*for (int i = 0; i < _countof(worldTransform_); i++) {
	    model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}*/

	for (int i = PartId::Chest; i <= PartId::ArmR; i++) {
		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}

	for (int i = 50; i <= 53; i++) {
		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	// sprite_->Draw();

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
