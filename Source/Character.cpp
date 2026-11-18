#include "Character.h"

//行列更新処理
void Character::UpdateTransform()
{
	//スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//
	DirectX::XMMATRIX W = S * R * T;
	//
	DirectX::XMStoreFloat4x4(&transform, W);
}

//デバッグプリミティブ描画
void Character::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	//renderer->RenderSphere(rc, position, radius, DirectX::XMFLOAT4( 0, 0, 0, 1 ));
	// 衝突判定用のデバッグ円柱を描画
	renderer->RenderCylinder(rc, position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}


void Character::Move(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;
	position.x += vx * speed;
	position.z += vz * speed;
}

//旋回処理
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	float length = sqrtf(vx * vx + vz * vz);

	if (length < 0.001f)return;

	vx /= length;
	vz /= length;

	//回転角を求めるため、2つの単位ベクトルの内積を計算する
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);
	//内積値は-1.0～1.0で表現されており、２つの
	float dot = (frontZ * vz) + (frontX * vx);

	float rot = 1.0 - dot;
	if (rot > speed)rot = speed;

	float cross = (frontZ * vx) - (frontX * vz);

	if (cross < 0.0f)
	{
		angle.y -= speed;
	}

	else
	{
		angle.y += speed;
	}

}

//ジャンプ処理
void Character::Jump(float speed) {

	//上方向の力を設定
	velocity.y = speed;
}

//速力処理更新
void Character::UpdateVelocity(float elapsedTime) {

	//重力処理
	velocity.y += gravity * elapsedTime;//(30.0f * ( 1 / 60 ) )

	//移動処理
	position.y += velocity.y * elapsedTime;//(12.0f -0.5 (重力) * ( 1 / 60) )

	//地面判定
	if (position.y < 0.0f) {

		position.y = 0.0f;
		velocity.y = 0.0f;

		//着地した
		if (!isGround)
		{
			OnLanding();     // 着地時に呼ばれる関数を実行
			isGround = true; // 接地フラグをONにする
		}
		
	}
	else
	{
		// 空中にいるとき
		isGround = false;
	}

}