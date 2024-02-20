#pragma once
#include <vector>
#include <unordered_map>
#include <typeinfo>

#include "Component.h"
#include "ComponentContainer.h"
#include "../entity/Entity.h"

class IComponentContainer;
class World;

class ComponentManager
{
public:
	friend class World;

	ComponentManager()
	{
		myComponentContainers = {};
		myNextComponentId = 0;
	}

	void Reset(EntityManager* aManager)
	{
		for (auto& c : myComponentContainers)
		{
			c->Reset(aManager);
		}
	}

	template<typename T, size_t MAX_SIZE = MAX_ENTITIES>
	void RegisterComponent()
	{
		T::componentId = myNextComponentId;
		myComponentContainers.push_back(new ComponentContainer<T>(MAX_SIZE));

		++myNextComponentId;
	}

	template<typename T>
	T& AddComponent(const Entity& aEntity, EntityManager* aManager)
	{
		return GetComponentContainer<T>()->AddComponent(aEntity, aManager->GetSignature(aEntity));
	}

	template<typename T>
	void RemoveComponent(const Entity& aEntity, EntityManager* aManager)
	{
		GetComponentContainer<T>()->RemoveComponent(aEntity, aManager);
	}

	template<typename T>
	T& GetComponent(const Entity& aEntity, EntityManager* aManager) const
	{
		return GetComponentContainer<T>()->GetComponent(aManager->GetSignature(aEntity));
	}

	template<typename T>
	T* TryGetComponent(const Entity& aEntity, EntityManager* aManager) const
	{
		return GetComponentContainer<T>()->TryGetComponent(aManager->GetSignature(aEntity));
	}

	void OnEntityDestroyed(const Entity& aEntity, EntityManager* aManager)
	{
		for (auto& c : myComponentContainers)
		{
			c->OnEntityDestroyed(aEntity, aManager);
		}
	}

	template<typename T>
	cid_t GetComponentSignatureID() const
	{
		return T::componentId;
	}
private:

	std::vector<IComponentContainer*> myComponentContainers;
	cid_t myNextComponentId = 0;

	template<typename T>
	ComponentContainer<T>* GetComponentContainer() const
	{
		cid_t cid = T::componentId;

		assert(cid < myNextComponentId && "Component not registered before use.");

		return static_cast<ComponentContainer<T>*>(myComponentContainers.at(cid));
	}
};

