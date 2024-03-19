#pragma once
#include <ecs\component\Component.h>

struct OrbComponent : public Component<OrbComponent>
{
	float HPRegainTimer = 0;
	float HPRegaintime = 5.f;

	float ChaseSpeed = 8.f;
	Vector3f OrbPos = { 0,0,0 };
};