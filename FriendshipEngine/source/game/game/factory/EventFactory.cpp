#include "pch.h"
#include "EventFactory.h"

#include <ecs/World.h>
#include <ecs/entity/Entity.h>

#include "../component/EventComponent.h"
#include "../component/ScriptableEventComponent.h"

#ifdef _DEBUG
#include "../component/MetaDataComponent.h"
#endif // !_DEBUG


EventFactory::EventFactory()
{
	myWorld = nullptr;
	myInstance = nullptr;
}


EventFactory& EventFactory::GetInstance()
{
	if (myInstance == nullptr)
	{
		myInstance = new EventFactory();
	}
	return *myInstance;
}


void EventFactory::Init(World* aWorld)
{
	myWorld = aWorld;
}

Entity EventFactory::CreateEvent(eEvent aEventToTrigger, Entity aOwner)
{
	Entity eventEntity = myWorld->CreateEntity();
	auto& eventComponent = myWorld->AddComponent<EventComponent>(eventEntity);

#ifdef _DEBUG
	auto& metaDataComponent = myWorld->AddComponent<MetaDataComponent>(eventEntity);
	 std::strcpy(metaDataComponent.name, "Event");
#endif
	// ADD ALL AMOUNTS OF COMPONENTS

	eventComponent.eventToTrigger = aEventToTrigger;
	eventComponent.ownerID = aOwner;

	auto& ownerScriptableComponent = myWorld->GetComponent<ScriptableEventComponent>(aOwner);
	ownerScriptableComponent.isInProgress = true;
	eventComponent.targetID = ownerScriptableComponent.triggerEntityID;

	return eventEntity;
}