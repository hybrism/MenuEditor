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

	void Reset(EntitySignatureManager* aManager)
	{
		for (auto& c : myComponentContainers)
		{
			c->Reset(aManager);
		}
	}

	template<typename T>
	void RegisterComponent(size_t aMaxSize)
	{
		T::componentId = myNextComponentId;
		myComponentContainers.push_back(new ComponentContainer<T>(aMaxSize));

		++myNextComponentId;
	}

	template<typename T>
	T& AddComponent(const Entity& aEntity, EntitySignatureManager* aManager)
	{
		return GetComponentContainer<T>()->AddComponent(aEntity, aManager);
	}

	template<typename T>
	void RemoveComponent(const Entity& aEntity, EntitySignatureManager* aManager)
	{
		GetComponentContainer<T>()->RemoveComponent(aEntity, aManager);
	}

	template<typename T>
	T& GetComponent(const Entity& aEntity, EntitySignatureManager* aManager) const
	{
		assert(aManager->GetComponentSignature(aEntity).test(T::componentId) && "Entity does not exist in ComponentContainer");
		return GetComponentContainer<T>()->GetComponent(aManager->GetSignatureIndex(aEntity, T::componentId));
	}

	template<typename T>
	T* TryGetComponent(const Entity& aEntity, EntitySignatureManager* aManager) const
	{
		if (!aManager->GetComponentSignature(aEntity).test(T::componentId)) { return nullptr; }
		return &GetComponent<T>(aEntity, aManager);
	}

	void OnEntityDestroyed(const Entity& aEntity, EntitySignatureManager* aManager)
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

	template<typename T>
	bool HasComponent(const Entity& aEntity, EntitySignatureManager* aManager) const
	{
		return aManager->GetComponentSignature(aEntity)[GetComponentSignatureID<T>()];
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

