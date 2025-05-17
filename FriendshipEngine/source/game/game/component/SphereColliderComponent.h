#pragma once
#include <ecs\component\Component.h>
#include <engine\math\Vector.h>


struct SphereColliderComponent : public Component<SphereColliderComponent>
{
	float radius;
	Vector3<float> center;
};