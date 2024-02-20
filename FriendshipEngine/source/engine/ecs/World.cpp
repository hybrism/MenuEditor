#include "World.h"

#include "entity/EntityManager.h"
#include "component/ComponentManager.h"
#include "system/SystemManager.h"

World::~World()
{
	delete myEntityManager;
	delete myComponentManager;
	delete mySystemManager;
}

void World::Reset()
{
	myComponentManager->Reset(myEntityManager);
	myEntityManager->Reset();
	mySystemManager->Reset();

	myPlayerEntityID = INVALID_ENTITY;

#ifdef _DEBUG
	myEntities.clear();
#endif
}

void World::Init()
{
	// register all the components
//REGISTER_COMPONENT(CTransform);
	myEntityManager = new EntityManager();
	mySystemManager = new SystemManager(this);
	myComponentManager = new ComponentManager();
}

void World::Update(const float& dt)
{
	mySystemManager->Update(dt);
	DeleteMarkedEntities();
}

void World::Render()
{
	mySystemManager->Render();
}

Entity World::CreateEntity()
{
#ifdef _DEBUG
	eid_t entity = myEntityManager->CreateEntity();
	myEntities.push_back(entity);
	return entity;
#else
	return myEntityManager->CreateEntity();
#endif
}

Entity World::CreateEntityAtID(const eid_t& aEntityID)
{
#ifdef _DEBUG
	eid_t entity = myEntityManager->CreateEntityAtID(aEntityID);
	myEntities.push_back(entity);
	return entity;
#else
	return myEntityManager->CreateEntityAtID(aEntityID);
#endif
}

void World::DestroyEntity(const Entity& aEntity)
{
	MarkEntityForDeletion(aEntity);
	//myEntityManager->DestroyEntity(entity);
	//myComponentManager->OnEntityDestroyed(entity);
	//mySystemManager->OnEntityDestroyed(entity);

#ifdef _DEBUG
	auto index = std::find(myEntities.begin(), myEntities.end(), aEntity);
	if (index == myEntities.end()) { return; }
	myEntities.erase(index);
#endif
}

void World::InitSystems()
{
	mySystemManager->Init();
}

void World::MarkEntityForDeletion(const Entity& aEntity)
{
	myMarkedForDeletionEntities[myMarkedDeleteCount] = aEntity;
	myMarkedDeleteCount++;
}

void World::DeleteMarkedEntities()
{
	for (size_t i = 0; i < myMarkedDeleteCount; i++)
	{
		myEntityManager->DestroyEntity(myMarkedForDeletionEntities[i]);
		myComponentManager->OnEntityDestroyed(myMarkedForDeletionEntities[i], myEntityManager);
		mySystemManager->OnEntityDestroyed(myMarkedForDeletionEntities[i]);
	}
	myMarkedDeleteCount = 0;
}
