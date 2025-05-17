#include "pch.h"
#include "EventSystem.h"
#include <ecs/World.h>
#include <engine/utility/Error.h>

#include "../component/EventComponent.h"
#include "../component/PlayerComponent.h"
#include "../component/TransformComponent.h"
#include "../component/CollisionDataComponent.h"

#include "scene/SceneManager.h"
#include "scene/SceneCommon.h"
#include "scene/utility/GameTimer.h"

#include "../gui/MenuHandler.h"
#include "../gui/MenuObject.h"
#include "../gui/components/TextComponent.h"

#include <ecs/entity/Entity.h>


EventSystem::EventSystem(World* aWorld) : System(aWorld)
{
}

void EventSystem::Update(SceneUpdateContext& aContext)
{
	for (Entity entity : myEntities)
	{
		auto& eventComponent = myWorld->GetComponent<EventComponent>(entity);
		auto& collisionComp = myWorld->GetComponent<CollisionDataComponent>(entity);

		if (collisionComp.isColliding && !eventComponent.isComplete)
		{
			eventComponent.isComplete = true;
			PrintI("An event is Triggered!");

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

					playerComponent.SpawnRot = { pitch * -Rad2Deg , yaw * Rad2Deg };
					playerComponent.SpawnPoint = { spawnMatrix.r[3].m128_f32[0],spawnMatrix.r[3].m128_f32[1] ,spawnMatrix.r[3].m128_f32[2] };

				}
				break;
			}
			case eEvent::LoadLevel_3:
			{
				SceneParameter param;
				param.sceneType = eSceneType::Game;
				param.gameLevel = eLevel::Lvl3;
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
				param.gameLevel = eLevel::Lvl0;
				mySceneManager->LoadScene(param);
				break;
			}
			case eEvent::WinGame:
			{
				SceneParameter param;
				param.sceneType = eSceneType::MainMenu;
				param.gameLevel = eLevel::None;
				mySceneManager->LoadScene(param);
				break;
			}
			case eEvent::LvlSelect:
			{
				std::string currentScene = mySceneManager->GetCurrentSceneName();
				mySceneManager->TogglePaused();

				if (currentScene == "lvl_0.json" || currentScene == "lvl_0")
				{
					aContext.menuHandler->PushState("Winscreen_0");
					MENU::MenuObject& scoreObject = aContext.menuHandler->GetObjectFromName("Score_0");
					MENU::TextComponent& text = scoreObject.GetComponent<MENU::TextComponent>();
					text.SetText(aContext.gameTimer->GetTimeAsString());
				}

				if (currentScene == "lvl_1.json" || currentScene == "lvl_1")
				{
					aContext.menuHandler->PushState("Winscreen_1");
					MENU::MenuObject& scoreObject = aContext.menuHandler->GetObjectFromName("Score_1");
					MENU::TextComponent& text = scoreObject.GetComponent<MENU::TextComponent>();
					text.SetText(aContext.gameTimer->GetTimeAsString());
				}

				if (currentScene == "lvl_2.json" || currentScene == "lvl_2")
				{
					aContext.menuHandler->PushState("Winscreen_2");
					MENU::MenuObject& scoreObject = aContext.menuHandler->GetObjectFromName("Score_2");
					MENU::TextComponent& text = scoreObject.GetComponent<MENU::TextComponent>();
					text.SetText(aContext.gameTimer->GetTimeAsString());
				}

				if (currentScene == "lvl_3.json" || currentScene == "lvl_3")
				{
					aContext.menuHandler->PushState("Winscreen_3");
					MENU::MenuObject& scoreObject = aContext.menuHandler->GetObjectFromName("Score_3");
					MENU::TextComponent& text = scoreObject.GetComponent<MENU::TextComponent>();
					text.SetText(aContext.gameTimer->GetTimeAsString());
				}

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