#pragma once
#include <ecs/component/Component.h>

struct HitboxComponent : public Component<HitboxComponent>
{
	float lifetime = 1.5f;
	float timer = 0;

	bool shouldDestroyOnCollide = false;
	bool didICollideLastFrame = false;
};