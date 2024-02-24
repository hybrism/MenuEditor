#pragma once

#include "entity/Entity.h"
#include "entity/EntityManager.h"

#include "component/ComponentManager.h"
#include "system/SystemManager.h"
#include <cassert>

#ifdef _DEBUG
#include <vector>
#endif

class System;

// ACTS AS ADAPTER FOR ALL MANAGERS
class World
{
public:
	World() = default;
	~World();

	void Reset();
	void Init();
	void Update(const float& aDeltaTime);
	void Render();

	// Entity Adapter
	Entity CreateEntity();
	Entity CreateEntityAtID(const eid_t& aEntityID);
	void DestroyEntity(const Entity& aEntity);

	void SetPlayerEntityID(const eid_t& aPlayerEntityID) { myPlayerEntityID = aPlayerEntityID; };
	eid_t GetPlayerEntityID() const
	{
		//Value will be INVALID_ENTITY if no player exists in scene
		return myPlayerEntityID;
	};

	// Component Adapter
	template<typename T, size_t MAX_SIZE = MAX_ENTITIES>
	void RegisterComponent();

	template<typename T>
	T& AddComponent(const Entity& aEntity);

	template<typename T>
	void RemoveComponent(const Entity& aEntity);

	template<typename T>
	T& GetComponent(const Entity& aEntity);

	// System Adapter
	template<typename T, typename... Param>
	T* RegisterSystem(Param... aParams);

	template<typename T, typename... Param>
	T* RegisterSystem(const ComponentSignature& aSignature, Param... aParams);

	template<typename T>
	void SetSystemSignature(const ComponentSignature& aSignature);

	template<typename T>
	cid_t GetComponentSignatureID();

	template<typename T>
	T* GetSystem();

	void InitSystems();

	template<typename T>
	T* TryGetComponent(const Entity& aEntity);

#ifdef _DEBUG
	std::vector<Entity>& GetEntities() { return myEntities; }
#endif

private:
	World(const World& aWorld) = delete;
	World& operator=(const World& aOther) = delete;

	template<typename T>
	void UpdateSignature(const Entity& aEntity, const bool& aValue);

	void MarkEntityForDeletion(const Entity& entity);
	void DeleteMarkedEntities();

	std::array<Entity, MAX_ENTITIES> myMarkedForDeletionEntities;
	size_t myMarkedDeleteCount = 0;

	EntityManager* myEntityManager = nullptr;
	SystemManager* mySystemManager = nullptr;
	ComponentManager* myComponentManager = nullptr;

	eid_t myPlayerEntityID = INVALID_ENTITY;

#ifdef _DEBUG
	std::vector<Entity> myEntities = {};
#endif
};

template<typename T, size_t MAX_SIZE>
void World::RegisterComponent()
{
	myComponentManager->RegisterComponent<T, MAX_SIZE>();
}

template<typename T>
T& World::AddComponent(const Entity& aEntity)
{
	T& component = myComponentManager->AddComponent<T>(aEntity, myEntityManager);
	UpdateSignature<T>(aEntity, true);
	return component;
}

template<typename T>
void World::RemoveComponent(const Entity& aEntity)
{
	myComponentManager->RemoveComponent<T>(aEntity, myEntityManager);
	UpdateSignature<T>(aEntity, false);
}

template<typename T>
inline T& World::GetComponent(const Entity& aEntity)
{
	return myComponentManager->GetComponent<T>(aEntity, myEntityManager);
}

template<typename T, typename... Param>
T* World::RegisterSystem(Param... aParams)
{
	return mySystemManager->RegisterSystem<T>(aParams...);
}
template<typename T>
T* World::GetSystem()
{
	return mySystemManager->GetSystem<T>();
}

template<typename T, typename... Param>
inline T* World::RegisterSystem(const ComponentSignature& aSignature, Param... aParams)
{
	T* system = World::RegisterSystem<T>(aParams...);
	SetSystemSignature<T>(aSignature);
	return system;
}

template<typename T>
inline void World::SetSystemSignature(const ComponentSignature& aSignature)
{
	mySystemManager->SetSignature<T>(aSignature);
}

template<typename T>
inline cid_t World::GetComponentSignatureID()
{
	return myComponentManager->GetComponentSignatureID<T>();
}

template<typename T>
void World::UpdateSignature(const Entity& aEntity, const bool& aValue)
{
	myEntityManager->UpdateSignature(aEntity, myComponentManager->GetComponentSignatureID<T>(), aValue);
	mySystemManager->OnEntitySignatureChanged(aEntity, myEntityManager->GetSignature(aEntity).signature);
}

#ifdef _DEBUG
template<typename T>
inline T* World::TryGetComponent(const Entity& aEntity)
{
	return myComponentManager->TryGetComponent<T>(aEntity, myEntityManager);
}
#endif
