#include "pch.h"
#include "ScriptableEventSystem.h"

#include <ecs/World.h>
#include "../factory/EventFactory.h"

#include "../component/CollisionDataComponent.h"
#include "../component/ScriptableEventComponent.h"



ScriptableEventSystem::ScriptableEventSystem(World* aWorld) : System(aWorld)
{
}


void ScriptableEventSystem::Update(const float&)
{

	for (auto entity : myEntities)
	{
		auto& collisionData = myWorld->GetComponent<CollisionDataComponent>(entity);
		auto& scriptableEvent = myWorld->GetComponent<ScriptableEventComponent>(entity);

		if (collisionData.isColliding == false)
			continue;
		if (scriptableEvent.isInProgress == true)
			continue;

		EventFactory::GetInstance().CreateEvent(scriptableEvent.eventToTrigger, entity);

	}

}
