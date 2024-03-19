#pragma once
#include "IComponentContainer.h"
#include "../entity/Entity.h"
#include "../entity/EntitySignatureManager.h"

#include <cassert>
#include <set>
#include <unordered_map>
#include <vector>

template<typename Derived>
struct Component;

template<typename T>
class ComponentContainer : public IComponentContainer
{
public:
	ComponentContainer(size_t aMaxSize) : myMaxSize(aMaxSize), myComponentToEntityMap()
	{
		// since we want segmented memory we block allocate an array, additionally the defai;t size is MAX_ENTITES since it is possible for every entity to have every component
		myComponents = new T[aMaxSize];
		myComponentToEntityMap = new eid_t[aMaxSize];
		mySize = 0;
	}

	~ComponentContainer() override
	{
		delete[] myComponents;
		delete[] myComponentToEntityMap;
		mySize = 0;
	}

	void Reset(EntitySignatureManager* aManager) override
	{
		while (mySize > 0)
		{
			RemoveComponentInternal(myComponentToEntityMap[mySize - 1], aManager);
		}
	}

	T& AddComponent(const Entity& aEntity, EntitySignatureManager* aManager)
	{
		assert(mySize < myMaxSize && "ComponentContainer is full");
		assert(!aManager->GetComponentSignature(aEntity).test(T::componentId) && "Entity contains the component you are trying to add");

		aManager->SetSignatureIndex(aEntity, T::componentId, static_cast<int>(mySize));
		aManager->UpdateComponentSignature(aEntity, T::componentId, true);
		myComponentToEntityMap[mySize] = aEntity;
		++mySize;

		return myComponents[mySize - 1];
	}

	void RemoveComponent(const Entity& aEntity, EntitySignatureManager* aManager)
	{
		assert(aManager->GetComponentSignature(aEntity).test(T::componentId) && "Entity does not exist in ComponentContainer");
		RemoveComponentInternal(aEntity, aManager);
	}

	T& GetComponent(int aIndex) const
	{
		assert(aIndex < mySize && "Index out of range");
		return myComponents[aIndex];
	}

	void OnEntityDestroyed(const Entity& aEntity, EntitySignatureManager* aManager) override
	{
		// if the entity is not in the container, do nothing
		if (!aManager->GetComponentSignature(aEntity).test(T::componentId)) { return; }

		RemoveComponent(aEntity, aManager);
	}

	const size_t& GetMaxSize() const { return myMaxSize; }

//#ifndef _RELEASE
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
	void RemoveComponentInternal(const Entity& aEntity, EntitySignatureManager* aManager)
	{
		assert(mySize > 0 && "ComponentContainer is empty");

		size_t lastElementIndex = mySize - 1;
		int indexOfRemovedEntity = aManager->GetSignatureIndex(aEntity, T::componentId);
		eid_t lastEntity = myComponentToEntityMap[lastElementIndex];

		memcpy(&myComponents[indexOfRemovedEntity], &myComponents[lastElementIndex], sizeof(myComponents[indexOfRemovedEntity]));
		T copy{};
		memcpy(&myComponents[lastElementIndex], &copy, sizeof(T));  // resets the component

		aManager->SetSignatureIndex(lastEntity, T::componentId, indexOfRemovedEntity);
		aManager->UpdateComponentSignature(aEntity, T::componentId, false);
		--mySize;
	}

	const size_t myMaxSize;
	size_t mySize;
	T* myComponents;
	eid_t* myComponentToEntityMap;
	// TODO: optimize initialization of components if they contain too lot of data?
	
#ifndef _RELEASE
	std::vector<EntityData*> myEntities;
#endif
};

