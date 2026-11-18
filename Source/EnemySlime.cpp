#include"EnemySlime.h"

//コンストラクタ
EnemySlime::EnemySlime()
{
	//モデルを読み込み
	model = new Model("Data/Model/Slime/Slime.mdl");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.01f;

	radius = 0.5f;
	height = 1.0f;
}

//デストラクタ
EnemySlime::~EnemySlime()
{
	//モデルを破棄
	delete model;
}

//更新処理
void EnemySlime::Update(float elapsedTime)
{
	//速力更新処理
	UpdateVelocity(elapsedTime);

	//行列更新処理
	UpdateTransform();
	
	//モデルの行列更新処理
	model->UpdateTransform();
}

//描画処理
void EnemySlime::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);
}