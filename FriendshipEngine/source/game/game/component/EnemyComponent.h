#pragma once
#include <ecs/component/Component.h>


enum class eEnemyState
{
	Recoil,
	count
};

struct EnemyComponent : public Component<EnemyComponent>
{
	float attackSpeed = 1.3f;
	float attackSpeedTimer = 0;
	eEnemyState enemyState = eEnemyState::Recoil;
};