#pragma once
#include <ecs/component/Component.h>
#include "../collision/CollisionLayers.h"

enum class eCollisionType
{
	Collider,
	Trigger,

	Size
};



struct DamageData
{
	int damage = 0;
	bool isStagger = false;
	std::vector<eid_t> targetIDs;
	//eid_t targetID = INVALID_ENTITY;
};

struct RestorationData
{
	int healAmount = 0;
	int manaAmount = 0;
};


struct CollisionDataComponent : public Component<CollisionDataComponent>
{
	eCollisionType type = eCollisionType::Size;
	eCollisionLayer layer = eCollisionLayer::Everything;

	
	
	eid_t ownerID = INVALID_ENTITY;

	bool isColliding = false;
	bool isPrevFrameColliding = false;
	DamageData damageData;
	RestorationData restorationData;
};