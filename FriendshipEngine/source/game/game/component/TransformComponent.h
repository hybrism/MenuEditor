#pragma once
#include <ecs/component/Component.h>
#include <engine/math/Transform.h>
#include <ecs/entity/Entity.h>
#include "../utility/GameHelperFunctions.h"
#include <array>
#include <functional>


#define MAX_CHILDREN 1064

// TODO: include local transform variable to reduce the amount of calculations needed in TransformSystem?
struct TransformComponent : public Component<TransformComponent>
{
	DirectX::XMMATRIX(*CalculateWorldTransform)(World*, Entity)  = [](World* aWorld, Entity aEntity)-> DirectX::XMMATRIX
		{
			return GameHelperFunctions::GetWorldTransform(aWorld, aEntity); 
		};

	const DirectX::XMMATRIX GetWorldTransform(World* aWorld, Entity aEntity)
	{
		return CalculateWorldTransform(aWorld, aEntity);
	};
	Transform transform = {};
	Entity parent = 0;
	std::array<Entity, MAX_CHILDREN> children = {};
protected:
	//bool myHasMoved = false;
private:

	friend class GameHelperFunctions;
};
