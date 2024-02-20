#include "pch.h"
#include "EventSystem.h"
#include <ecs/World.h>
#include <engine/utility/Error.h>

#include "../component/EventComponent.h"
#include "../component/PlayerComponent.h"
#include "../component/TransformComponent.h"

#include <ecs/entity/Entity.h>


EventSystem::EventSystem(World* aWorld) : System(aWorld)
{
}



void EventSystem::Update(const float&)
{
	for (Entity entity : myEntities)
	{
		auto& eventComponent = myWorld->GetComponent<EventComponent>(entity);

		switch (eventComponent.eventToTrigger)
		{
#pragma region			WhichEventToTrigger


		case eEvent::OpenDoor:
		{
			break;
		}

		case eEvent::LevelUp:
		{

			break;
		}
		case eEvent::Checkpoint:
		{
			auto playerID = myWorld->GetPlayerEntityID();
			if (playerID != INVALID_ENTITY)
			{
				//auto& playerComponent = myWorld->GetComponent<PlayerComponent>(playerID);
				//auto& newCheckpointPosition = myWorld->GetComponent<TransformComponent>(eventComponent.targetID).transform;
				//playerComponent.mySpawnPosition = newCheckpointPosition.GetPosition();
			}
			eventComponent.isComplete = true;
			break;
		}
		case eEvent::Cutscene:
		{
			break;
		}
		case eEvent::BossDead:
		{

			break;
		}
		case eEvent::UnlockAbilityLMB:
		{

			break;
		}
		case eEvent::UnlockAbilityRMB:
		{

			break;
		}
		case eEvent::UnlockAbilityAOE:
		{

			break;
		}
		case eEvent::UnlockAbilityDash:
		{

			break;
		}
		case eEvent::UnlockAbilityShield:
		{

			break;
		}
		case eEvent::UnlockAbilityUltimate:
		{
			break;
		}
		default:
		{
			PrintE("An event is sent without a Event to Trigger!");
			break;
		}
		}
#pragma endregion

		// After the event has done it's work, it will be deleted. Also delete the Trigger collider aswell
		if (eventComponent.isComplete)
		{
			myWorld->DestroyEntity(entity);
			myWorld->DestroyEntity(eventComponent.ownerID);
		}
	}
}

void EventSystem::Connect(StateStack* aStateStack)
{
	myStateStackPtr = aStateStack;
}
