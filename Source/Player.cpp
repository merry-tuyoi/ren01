#include "Player.h"
#include"System/Input.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "Collision.h"
#include<imgui.h>
#include "ProjectileStraight.h"

//コンストラクタ
Player::Player()
{
	model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

	//
	scale.x = scale.y = scale.z = 0.01f;
}

//デストラクタ
Player::~Player()
{
	delete model;
}

//
void Player::Update(float elapsedTime)
{
	////進行ベクトル取得
	//DirectX::XMFLOAT3 moveVec = GetMoveVec();

	////移動処理
	//float moveSpeed = this->moveSpeed * elapsedTime;
	//position.x += moveVec.x * moveSpeed * elapsedTime;
	//position.z += moveVec.z * moveSpeed * elapsedTime;

	////入力情報を取得
	//GamePad& gamePad = Input::Instance().GetGamePad();
	//float ax = gamePad.GetAxisLX();
	//float ay = gamePad.GetAxisLY();

	////移動処理
	//float moveSpeed = this->moveSpeed * elapsedTime;
	//{
	//	//左スティックの入力値をもとにXZ平面への移動処理
	//	position.x += ax * moveSpeed;
	//	position.z += ay * moveSpeed;
	//}

	//
	////回転処理
	//float rotateSpeed = DirectX::XMConvertToRadians(360) * elapsedTime;
	//if (gamePad.GetButton() & GamePad::BTN_A)
	//{
	//	//X軸回転
	//	angle.x += rotateSpeed;
	//}

	//if (gamePad.GetButton() & GamePad::BTN_B)
	//{
	//	//Y軸回転
	//	angle.y += rotateSpeed;
	//}

	//if (gamePad.GetButton() & GamePad::BTN_X)
	//{
	//	//Z軸回転
	//	angle.z += rotateSpeed;
	//}

	//移動入力処理
	InputMove(elapsedTime);

	//ジャンプ入力処理
	InputJump();

	// 弾丸入力処理
	InputProjectile();

	//速力処理更新
	UpdateVelocity(elapsedTime);


	//弾丸更新処理
	projectileManager.Update(elapsedTime);


	//プレイヤーとエネミーの衝突処理
	CollisionPlayerVsEnemies();

	//
	UpdateTransform();

	//
	model->UpdateTransform();




}




// 弾丸入力処理
void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	// 直進弾丸発射
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		// 前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);
			// 発射位置（プレイヤーの腰あたり）
			DirectX::XMFLOAT3 pos;
			pos.x = position.x;
			pos.y = position.y + height * 0.5f;
			pos.z = position.z;
			// 発射
			ProjectileStraight * projectile = new ProjectileStraight();
		projectile->Launch(dir, pos);
		projectileManager.Register(projectile);
	}
}


//デバッグ用GUI描画
void Player::DrawDebugGUI()
{
	ImVec2 pos =ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if(ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		//トランスフォーム
		if(ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//位置
			ImGui::DragFloat3("Position", &position.x);
			//回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			//スケール
			ImGui::DragFloat3("Scale", &scale.x);
		}
	}
	ImGui::End();
}



//移動入力処理
void Player::InputMove(float elapsedTime)
{
	//進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();
	//移動処理
	Move(elapsedTime, moveVec.x, moveVec.z, moveSpeed);
	//旋回処理
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}


//描画処理
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);

	projectileManager.Render(rc, renderer);
		
}
// デバッグプリミティブ描画
void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	// 基底クラスの関数呼び出し
	Character::RenderDebugPrimitive(rc, renderer);
	// 弾丸デバッグプリミティブ描画
	projectileManager.RenderDebugPrimitive(rc, renderer);
}


DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	//入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//カメラの方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//移動ベクトルはXZ平面に水平なベクトルになるようにする
	
	//カメラ方向ベクトルをXZ単位に変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		//単位ベクトル化
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	//カメラ前方向ベクトルをXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		//単位ベクトル化
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	//スティックの水平入力値をカメラ右方向に反映し、
	//スティックの垂直入力値をカメラ前方向に反映し、
	//進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	//Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;
}

//プレイヤーとエネミーの衝突処理
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();
	for(int i = 0; i < enemyCount; i++)	
	{
		Enemy* enemy = enemyManager.GetEnemy(i);
		//衝突処理
		DirectX::XMFLOAT3 outposition;
		if (Collision::IntersectCylinderVsCylinder
		(
			position,
			radius,
			height,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outposition))
		{
			if (Collision::IntersectCylinderVsCylinder
			(
				position, radius, height,        //A（プレイヤー）
				enemy->GetPosition(),            //B（敵）
				enemy->GetRadius(),
				enemy->GetHeight(),
				outposition
			)
				)
			{
				////B（敵）を押し出す
				//enemy->SetPosition(outposition);

				//敵の真上に付近で当たったか判定
				DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
				DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
				DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
				DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
				DirectX::XMFLOAT3 normal;
				DirectX::XMStoreFloat3(&normal, N);

				//上からなら小ジャンプ
				if (normal.y > 0.8f)
				{
					//小ジャンプ
					Jump(jumpSpeed * 0.5f);
				}
				else
				{
					//横からなら押し出す
					enemy->SetPosition(outposition);
				}


			}
		}
	}
}

////ジャンプ入力処理
//void Player::InputJump() {
//	
//	//ボタン入力でジャンプ（ジャンプ回数制限付き）
//	GamePad& gamePad = Input::Instance().GetGamePad();
//
//	//[Z]キー
//	if (gamePad.GetButtonDown() & GamePad::BTN_A) {
//		Jump(jumpSpeed);
//	}
//}

//着地したらよばれる
void Player::OnLanding()

{
	
	jumpCount = 0;
}

//ジャンプ入力処理
void Player::InputJump() 
{

	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		
		// ジャンプ回数が制限未満ならジャンプできる
		if (jumpCount < jumpLimit)
		{
			
			jumpCount++; // ジャンプ回数を1回増やす
			Jump(jumpSpeed);
		}
	}

}