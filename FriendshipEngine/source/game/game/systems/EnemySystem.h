#pragma once
#include "../component/EnemyComponent.h"

struct TransformComponent;
struct ProjectileComponent;
enum class eEnemyState;

class EnemySystem : public System
{
public:
	EnemySystem(World* aWorld);
	~EnemySystem() override;

	void Update(const float& aDT) override;
	void AimAtPlayer(TransformComponent& aTransform,const float& aDT);
	void SetEnemyState(eEnemyState aState);
	void ShootArrow(TransformComponent& aTransform);

private:
	Vector3f myPos;
	Vector3f myPlayerPos;
	Vector3f myDirection;
};


