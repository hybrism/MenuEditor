#pragma once
#include <ecs/component/Component.h>


enum class eEnemyState
{
	TargetingPlayer,
	Idle,
	count
};

struct EnemyComponent : public Component<EnemyComponent>
{
	float attackSpeed = 1.3f;
	float attackSpeedTimer = 0;
	float delayAttackTimer = 2.f;
	float lerpTimer = 2.f;
	float pickmegirl = 2.f;
	float range = 4000.f;
	bool IsTargetingPlayer = false;
	Vector3f myPos;
	Vector3f overlapShapePos;
	Vector3f sizeCollider;
	eEnemyState enemyState = eEnemyState::Idle;
};