#pragma once
#include <ecs/component/Component.h>


enum class eEnemyState
{

	Recoil,
	count
};

struct EnemyComponent : public Component<EnemyComponent>
{
	

	eEnemyState enemyState = eEnemyState::Recoil;

};