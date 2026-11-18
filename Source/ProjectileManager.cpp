#include "ProjectileManager.h"
// コンストラクタ
ProjectileManager::ProjectileManager()
{
}
// デストラクタ
ProjectileManager::~ProjectileManager()
{
	Clear();
}
// 更新処理
void ProjectileManager::Update(float elapsedTime)
{
	// 更新処理
	for (Projectile* projectile : projectiles)
	{
		projectile->Update(elapsedTime);
	}
}
// 描画処理
void ProjectileManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (Projectile* projectile : projectiles)
	{
		projectile->Render(rc, renderer);
	}
}
// デバッグプリミティブ描画
void ProjectileManager::RenderDebugPrimitive(const RenderContext& rc,
	ShapeRenderer* renderer)
{
	for (Projectile* projectile : projectiles)
	{
		projectile->RenderDebugPrimitive(rc, renderer);
	}
}
// 弾丸登録
void ProjectileManager::Register(Projectile* projectile)
{
	projectiles.emplace_back(projectile);
}

// 弾丸全削除
void ProjectileManager::Clear()
{
	for (Projectile* projectile : projectiles)
	{
		delete projectile;
	}
	projectiles.clear();
}