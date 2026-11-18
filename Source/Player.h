#pragma once

#include"System/ModelRenderer.h"
#include"Character.h"
#include "ProjectileManager.h"

//
class Player : public Character
{
public:
	Player();
	~Player() override;

	//
	void Update(float elapsedTime);

	//
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//デバッグGUI描画
	void DrawDebugGUI();

	//ジャンプ入力処理
	void InputJump();

	// デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer) override;
protected:
	// 着地した時に呼ばれる
	void OnLanding() override;

private:
	//スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;
	
	//移動入力処理
	void InputMove(float elapsedTime);

	//プレイヤーとエネミーの衝突処理
	void CollisionPlayerVsEnemies();

	// 弾丸入力処理
	void InputProjectile();


private:

	Model* model = nullptr;
	float	moveSpeed = 5.0f;
	float	turnSpeed = DirectX::XMConvertToRadians(720);

	float jumpSpeed = 12.0f;

	int jumpCount = 0;
	int jumpLimit = 2;
	ProjectileManager projectileManager;
};

