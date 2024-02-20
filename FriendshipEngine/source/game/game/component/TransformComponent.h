#pragma once
#include <ecs/component/Component.h>
#include <engine/math/Transform.h>
#include <ecs/entity/Entity.h>
#include <array>

#define MAX_CHILDREN 1064

// TODO: include local transform variable to reduce the amount of calculations needed in TransformSystem?
struct TransformComponent : public Component<TransformComponent>
{
	Transform transform = {};
	Entity parent = 0;
	std::array<Entity, MAX_CHILDREN> children = {};
};
