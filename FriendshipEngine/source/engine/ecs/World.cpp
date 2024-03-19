#include "World.h"

#include "entity/EntityManager.h"
#include "component/ComponentManager.h"
#include "system/SystemManager.h"


World::~World()
{
	delete myEntityManager;
	delete myEntitySignatureManager;
	delete myComponentManager;
	delete mySystemManager;
}

void World::Reset()
{
	myComponentManager->Reset(myEntitySignatureManager);
	myEntitySignatureManager->Reset();
	myEntityManager->Reset();
	mySystemManager->Reset();

	myPlayerEntityID = INVALID_ENTITY;

#ifdef _EDITOR
	myEntities.clear();
#endif
}

void World::Init()
{
	//register all the components
  //REGISTER_COMPONENT(CTransform);
	myEntitySignatureManager = new EntitySignatureManager();
	myEntityManager = new EntityManager();
	mySystemManager = new SystemManager(this);
	myComponentManager = new ComponentManager();
}

void World::Update(const SceneUpdateContext& aContext)
{
	mySystemManager->Update(aContext);
	DeleteMarkedEntities();
}

void World::Render()
{
	mySystemManager->Render();
}

Entity World::CreateEntity()
{
#ifdef _EDITOR
	eid_t entity = myEntityManager->CreateEntity();
	myEntities.push_back(entity);
	return entity;
#else
	return myEntityManager->CreateEntity();
#endif
}

Entity World::CreateEntityAtID(const eid_t& aEntityID)
{
#ifdef _EDITOR
	eid_t entity = myEntityManager->CreateEntityAtID(aEntityID);
	myEntities.push_back(entity);
	return entity;
#else
	return myEntityManager->CreateEntityAtID(aEntityID);
#endif
}

void World::DestroyEntity(const Entity& aEntity)
{
	if (aEntity == INVALID_ENTITY) { return; }

	MarkEntityForDeletion(aEntity);
	//myEntityManager->DestroyEntity(entity);
	//myComponentManager->OnEntityDestroyed(entity);
	//mySystemManager->OnEntityDestroyed(entity);

#ifdef _EDITOR
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
		myComponentManager->OnEntityDestroyed(myMarkedForDeletionEntities[i], myEntitySignatureManager);
		mySystemManager->OnEntityDestroyed(myMarkedForDeletionEntities[i]);
		myEntityManager->DestroyEntity(myMarkedForDeletionEntities[i], myEntitySignatureManager);
	}
	myMarkedDeleteCount = 0;
}
