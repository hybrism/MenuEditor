#pragma once
#include <ecs/component/Component.h>
#include <engine/math/Vector.h>
#include <engine/math/AABB3D.h>

struct ColliderComponent : public Component<ColliderComponent>
{
	Vector3f extents = { 0,0,0 };
	AABB3D<float> aabb3D;
	Vector3f color = {};
};