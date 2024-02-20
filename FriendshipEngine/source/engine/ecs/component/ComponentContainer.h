#pragma once
#include "IComponentContainer.h"
#include "../entity/Entity.h"
#include "../entity/EntityManager.h"

#include <cassert>
#include <set>
#include <unordered_map>
#include <vector>

// https://austinmorlan.com/posts/entity_component_system/

template<typename Derived>
struct Component;

template<typename T>
class ComponentContainer : public IComponentContainer
{
public:
	ComponentContainer(size_t MAX_SIZE) : myMaxSize(MAX_SIZE), myComponentToEntityMap()
	{
		// since we want segmented memory we use an array instead of a vector:(, additionally it is of the size of max entities since it is possible for every entity to have every component
		myComponents = new T[MAX_SIZE];
		myComponentToEntityMap = new eid_t[MAX_SIZE];
		mySize = 0;
	}

	~ComponentContainer() override
	{
		delete[] myComponents;
		delete[] myComponentToEntityMap;
		mySize = 0;
	}

	void Reset(EntityManager* aManager) override
	{
		while (mySize > 0)
		{
			RemoveComponent(myComponentToEntityMap[mySize - 1], aManager);
		}
	}

	T& AddComponent(const Entity& aEntity, EntitySignature& aSignature)
	{
		assert(mySize < myMaxSize && "ComponentContainer is full");
		assert(!aSignature.signature.test(T::componentId) && "Entity contains the component you are trying to add");

		aSignature.signature.set(T::componentId, true);
		aSignature.indices[T::componentId] = static_cast<int>(mySize);
		myComponentToEntityMap[mySize] = aEntity;
		++mySize;

		return myComponents[mySize - 1];
	}

	void RemoveComponent(const Entity& aEntity, EntityManager* aManager)
	{
		assert(aManager->GetSignature(aEntity).signature.test(T::componentId) && "Entity does not exist in ComponentContainer");
		assert(mySize > 0 && "ComponentContainer is empty");

		size_t lastElementIndex = mySize - 1;
		size_t indexOfRemovedEntity = aManager->GetSignature(aEntity).indices[T::componentId];
		eid_t lastEntity = myComponentToEntityMap[lastElementIndex];

		memcpy(&myComponents[indexOfRemovedEntity], &myComponents[lastElementIndex], sizeof(myComponents[indexOfRemovedEntity]));
		T copy{};
		memcpy(&myComponents[lastElementIndex], &copy, sizeof(T));  // resets the component
		aManager->GetSignature(lastEntity).indices[T::componentId] = static_cast<int>(indexOfRemovedEntity);

		aManager->GetSignature(aEntity).signature.set(T::componentId, false);

		--mySize;
	}

	T& GetComponent(EntitySignature& aSignature) const
	{
		assert(aSignature.signature.test(T::componentId) && "Entity does not exist in ComponentContainer");
		
		return myComponents[aSignature.indices[T::componentId]];
	}

#ifdef _DEBUG
	T* TryGetComponent(EntitySignature& aSignature) const
	{
		if (!aSignature.signature.test(T::componentId)) { return nullptr; }

		return &myComponents[aSignature.indices[T::componentId]];
	}
#endif

	void OnEntityDestroyed(const Entity& aEntity, EntityManager* aManager) override
	{
		// if the entity is not in the container, do nothing
		if (!aManager->GetSignature(aEntity).signature.test(T::componentId)) { return; }

		RemoveComponent(aEntity, aManager);
	}

	const size_t& GetMaxSize() const { return myMaxSize; }

//#ifdef _DEBUG
//	virtual void OnEntityDestroyed(EntityData& aEntity) override
//	{
//		// if the entity is not in the container, do nothing
//		if (std::find(myEntities.begin(), myEntities.end(), &aEntity) == myEntities.end()) { return; }
//
//		RemoveComponent(aEntity.id);
//		auto id = std::find(myEntities.begin(), myEntities.end(), &aEntity);
//		myEntities.at(id)->components.erase(std::remove(myEntities.at(id)->components.begin(), myEntities.at(id)->components.end(), typeid(T)), myEntities.at(id)->components.end());
//		myEntities.erase(id);
//	}
//#endif
private:
	const size_t myMaxSize;
	size_t mySize;
	T* myComponents;
	eid_t* myComponentToEntityMap;
	// TODO: optimize initialization of components if they contain too lot of data?
	
#ifdef _DEBUG
	std::vector<EntityData*> myEntities;
#endif
};

