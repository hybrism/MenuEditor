#pragma once
#include <ecs/component/Component.h>
#include <engine/math/Vector.h>


struct CameraComponent : public Component<CameraComponent>
{
	float moveSpeed = 400.f;
	float moveMultiplier = 3.f;
	float cameraMoveX = 8.f;
	float cameraMoveY = 5.f;
	bool activeMultiplier = false;
};