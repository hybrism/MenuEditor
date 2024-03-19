#include "pch.h"
#include "EventSystem.h"
#include <ecs/World.h>
#include <engine/utility/Error.h>

#include "../component/EventComponent.h"
#include "../component/PlayerComponent.h"
#include "../component/TransformComponent.h"
#include "scene\SceneManager.h"

#include <ecs/entity/Entity.h>
#include <component\CollisionDataComponent.h>


EventSystem::EventSystem(World* aWorld) : System(aWorld)
{
}

void EventSystem::Update(const SceneUpdateContext& /*dt*/)
{
	for (Entity entity : myEntities)
	{
		auto& eventComponent = myWorld->GetComponent<EventComponent>(entity);
		auto& collisionComp = myWorld->GetComponent<CollisionDataComponent>(entity);

		if (collisionComp.isColliding && !eventComponent.isComplete)
		{
			eventComponent.isComplete = true;

			#pragma region WhichEventToTrigger
			switch (eventComponent.eventToTrigger)
			{
				case eEvent::Checkpoint:
				{
					auto playerID = myWorld->GetPlayerEntityID();
					if (playerID != INVALID_ENTITY)
					{
						auto& playerComponent = myWorld->GetComponent<PlayerComponent>(playerID);
						auto spawnMatrix = myWorld->GetComponent<TransformComponent>(eventComponent.targetID).GetWorldTransform(myWorld, eventComponent.targetID);
						
						Vector3f forward = { spawnMatrix.r[2].m128_f32[0], spawnMatrix.r[2].m128_f32[1], spawnMatrix.r[2].m128_f32[2] };

						float yaw = atan2(forward.x, forward.z);
						float pitch = asin(forward.y);

						playerComponent.SpawnRot = { pitch * -Rad2Deg , yaw * Rad2Deg};
						playerComponent.SpawnPoint = { spawnMatrix.r[3].m128_f32[0],spawnMatrix.r[3].m128_f32[1] ,spawnMatrix.r[3].m128_f32[2] };

					}
					break;
				}
				case eEvent::LoadLevel_3:
				{
					SceneParameter param;
					param.sceneType = eSceneType::Game;
					param.gameLevel = eLevel::Lvl3_FamilyHeirloom;
					mySceneManager->LoadScene(param);
					break;
				}
				case eEvent::LoadLevel_2:
				{
					SceneParameter param;
					param.sceneType = eSceneType::Game;
					param.gameLevel = eLevel::Lvl2;
					mySceneManager->LoadScene(param);
					break;
				}
				case eEvent::LoadLevel_1:
				{
					SceneParameter param;
					param.sceneType = eSceneType::Game;
					param.gameLevel = eLevel::Lvl1;
					mySceneManager->LoadScene(param);
					break;
				}
				case eEvent::Lvl0_Tutorial:
				{
					SceneParameter param;
					param.sceneType = eSceneType::Game;
					param.gameLevel = eLevel::Lvl0_Tutorial;
					mySceneManager->LoadScene(param);
					break;
				}
				case eEvent::WinGame:
				{
					//You Win!
					break;
				}
				default:
				{
					PrintE("An event is sent without a Event to Trigger!");
					break;
				}
			}
			#pragma endregion
		}

		
		// After the event has done it's work, it will be deleted. Also delete the Trigger collider aswell
		if (eventComponent.isComplete)
		{
			myWorld->DestroyEntity(entity);
			myWorld->DestroyEntity(eventComponent.targetID);
		}
	}
}