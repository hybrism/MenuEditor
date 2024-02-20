#pragma once
#include <ecs/component/Component.h>
#include <engine/math/Vector.h>

struct ProjectileComponent : public Component<ProjectileComponent>
{
	Vector3f direction;
	Vector3f position;

	float speed = 5300.f;
	float size = 30.f;

	bool fire = false;
	bool hit = false;
};