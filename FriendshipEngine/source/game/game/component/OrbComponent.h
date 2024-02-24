#pragma once
#include <ecs\component\Component.h>

struct OrbComponent : public Component<OrbComponent>
{
	//HP vars
	int playerHealth = 2;
	int playerMaxHealth = 2;
	float HPRegainTimer = 0;
	float HPRegaintime = 5.f;




	float ChaseSpeed = 8.f;
	Vector3f OrbPos = { 0,0,0 };
	//float CatchingSpeed;
	//float RegainSpeed;
};