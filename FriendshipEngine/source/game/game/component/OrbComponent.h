#pragma once
#include <ecs\component\Component.h>
#include <engine\math\Vector.h>

struct OrbComponent : public Component<OrbComponent>
{
	float ChaseSpeed = 8.f;
	float CatchingSpeed;
	float RegainSpeed;
};