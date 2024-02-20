#pragma once
#include "../entity/Entity.h"

class EntityManager;

// interface in order to be stored in one vector within ComponentContainer.h
class IComponentContainer
{
public:
	virtual ~IComponentContainer() {}

	virtual void Reset(EntityManager* aManager) = 0;
	virtual void OnEntityDestroyed(const Entity& aEntity, EntityManager* aManager) = 0;
//#ifdef _DEBUG
//	virtual void OnEntityDestroyed(const EntityData& aEntity) = 0;
//#endif
};

