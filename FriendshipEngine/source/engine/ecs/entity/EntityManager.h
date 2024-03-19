#pragma once
#include <deque>
#include <array>
#include <bitset>
#include <cassert>

#include "Entity.h"
#include "../component/Component.h"
#include "EntitySignatureManager.h"

class EntityManager
{
public:
	EntityManager()
	{
		Reset();
	}

	~EntityManager()
	{
		myFreeEntities = {};
		mySize = 0;
	}

	void Reset()
	{
		myFreeEntities = {};
		for (eid_t i = 0; i < MAX_ENTITIES; ++i)
		{
			myFreeEntities.insert(myFreeEntities.begin() + i, i + INVALID_ENTITY + 1);
		}
		mySize = 0;
	}

	Entity CreateEntity()
	{
		assert(mySize < MAX_ENTITIES && "Too many entities in existence.");

		eid_t id = myFreeEntities.front();
		myFreeEntities.pop_front();
		++mySize;

		return Entity(id);
	}

	Entity CreateEntityAtID(const eid_t& aEntity)
	{
		assert(aEntity < MAX_ENTITIES && "Entity out of range.");
		assert(mySize < MAX_ENTITIES && "Too many entities in existence.");
		assert(std::find(myFreeEntities.begin(), myFreeEntities.end(), aEntity) != myFreeEntities.end() && "Entity already in use.");

		myFreeEntities.erase(std::remove(myFreeEntities.begin(), myFreeEntities.end(), aEntity), myFreeEntities.end());

		return aEntity;
	}

	void DestroyEntity(const Entity& aEntity, EntitySignatureManager* aEntitySignatureManager)
	{
		assert(aEntity < MAX_ENTITIES && "Entity out of range.");

		aEntitySignatureManager->ResetComponentSignature(aEntity);
		myFreeEntities.insert(myFreeEntities.end(), aEntity);
		--mySize;
	}
private:
	std::deque<eid_t> myFreeEntities;
	size_t mySize;
};
