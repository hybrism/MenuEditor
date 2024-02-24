#include "pch.h"
#include "GameScene.h"

#include <ecs/World.h>
#include <engine/utility/InputManager.h>
#include <nlohmann/json.hpp>

#include "../utility/JsonUtility.h"
#include "../scripting/ScriptManager.h"
#include "../scripting/ScriptRuntimeInstance.h"
#include "../scripting/ScriptUpdateContext.h"

#pragma region Systems and Components Includes
//Systems
#include "../systems/PlayerSystem.h"
#include "../systems/RenderingSystem.h"
#include "../systems/CollisionSystem.h"
#include "../systems/EnemySystem.h"
#include "../systems/HitboxSystem.h"
#include "../systems/ProjectileSystem.h"
#include "../systems/CameraSystem.h"
#include "../systems/AnimatorSystem.h"
#include "../systems/ProjectileSystem.h"
#include "../systems/EventSystem.h"
#include "../systems/ScriptableEventSystem.h"
#include "../systems/PhysXSystem.h"
#include "../systems/OrbSystem.h"
#include "../systems/NpcSystem.h"

//Components
#include "../component/PlayerComponent.h"
#include "../component/NpcComponent.h"
#include "../component/HitboxComponent.h"
#include "../component/ColliderComponent.h"
#include "../component/TransformComponent.h"
#include "../component/CollisionDataComponent.h"
#include "../component/MeshComponent.h"
#include "../component/AnimationDataComponent.h"
#include "../component/MetaDataComponent.h"
#include "../component/EventComponent.h"
#include "../component/ScriptableEventComponent.h"
#include "../component/EnemyComponent.h"
#include "../component/CameraComponent.h"
#include "../component/AnimationDataComponent.h"
#include "../component/ProjectileComponent.h"
#include "../component/PhysXComponent.h"
#include "../component/OrbComponent.h"

#pragma endregion

GameScene::GameScene()
{
	myType = eSceneType::Game;
	myWorld = new World();
}

void GameScene::Init(PhysXSceneManager& aPhysXManager)
{
	myWorld->Init();

	InitComponents();
	InitSystems(aPhysXManager);
}

bool GameScene::Update(float dt)
{
	myWorld->Update(dt);

	for (size_t i = 0; i < myScripts.size(); i++)
	{
		myScripts[i]->Update({ dt });
	}

	auto* im = InputManager::GetInstance();
	if (im->IsKeyHeld(VK_SHIFT) && im->IsKeyPressed(VK_ESCAPE))
	{
		return false;
	}

	return true;
}

void GameScene::Render()
{
	myWorld->Render();
}

void GameScene::InitComponents()
{
	myWorld->RegisterComponent<TransformComponent>();
	myWorld->RegisterComponent<MeshComponent>();
	myWorld->RegisterComponent<PlayerComponent, 1>();
	myWorld->RegisterComponent<ColliderComponent>();
	myWorld->RegisterComponent<CollisionDataComponent>();
	myWorld->RegisterComponent<MeshComponent>();
	myWorld->RegisterComponent<EnemyComponent, 32>();
	myWorld->RegisterComponent<HitboxComponent, 64>();
	myWorld->RegisterComponent<CameraComponent, 8>();
	myWorld->RegisterComponent<AnimationDataComponent, 64>();
	myWorld->RegisterComponent<PhysXComponent, 128>();
	myWorld->RegisterComponent<OrbComponent, 1>();
	myWorld->RegisterComponent<ProjectileComponent, 32>();
	myWorld->RegisterComponent<NpcComponent, 32>();
	myWorld->RegisterComponent<EventComponent>();
	myWorld->RegisterComponent<ScriptableEventComponent, 64>();

#ifdef _DEBUG
	myWorld->RegisterComponent<MetaDataComponent>();
#endif
}

void GameScene::InitSystems(PhysXSceneManager& aPhysXManager)
{
	//TODO: Move all of these to respective constructor

	// RenderingSystem
	{
		ComponentSignature renderingSystemSignature;
		renderingSystemSignature.set(myWorld->GetComponentSignatureID<TransformComponent>());
		renderingSystemSignature.set(myWorld->GetComponentSignatureID<MeshComponent>());
		myWorld->RegisterSystem<RenderingSystem>(renderingSystemSignature);
	}

	myWorld->RegisterSystem<PlayerSystem, PhysXSceneManager*>(&aPhysXManager);
	myWorld->RegisterSystem<CameraSystem>();
	myWorld->RegisterSystem<CollisionSystem>();
	myWorld->RegisterSystem<EnemySystem>();

	// HitboxSystem
	{
		ComponentSignature hitboxSystemSignature;
		hitboxSystemSignature.set(myWorld->GetComponentSignatureID<HitboxComponent>());
		hitboxSystemSignature.set(myWorld->GetComponentSignatureID<CollisionDataComponent>());
		myWorld->RegisterSystem<HitboxSystem>(hitboxSystemSignature);
	}

	// EventSystem
	{
		ComponentSignature eventSystemSignature;
		eventSystemSignature.set(myWorld->GetComponentSignatureID<EventComponent>());
		myWorld->RegisterSystem<EventSystem>(eventSystemSignature);
	}

	// AnimatorSystem
	{
		ComponentSignature animationSystemSignature;
		animationSystemSignature.set(myWorld->GetComponentSignatureID<AnimationDataComponent>());
		animationSystemSignature.set(myWorld->GetComponentSignatureID<MeshComponent>());
		myWorld->RegisterSystem<AnimatorSystem>(animationSystemSignature);
	}

	//ScriptableEventSystem
	{
		myWorld->RegisterSystem<ScriptableEventSystem>();
	}

	//PhysXSystem
	{
		myWorld->RegisterSystem<PhysXSystem, PhysXSceneManager*>(&aPhysXManager);
	}

	//OrbSystem
	{
		ComponentSignature orbSystemSignature;
		orbSystemSignature.set(myWorld->GetComponentSignatureID<OrbComponent>());
		orbSystemSignature.set(myWorld->GetComponentSignatureID<TransformComponent>());
		myWorld->RegisterSystem<OrbSystem>(orbSystemSignature);
	}

	//ProjectileSystem
	{
		ComponentSignature projectileSystemSignature;
		projectileSystemSignature.set(myWorld->GetComponentSignatureID<ProjectileComponent>());
		projectileSystemSignature.set(myWorld->GetComponentSignatureID<TransformComponent>());
		myWorld->RegisterSystem<ProjectileSystem>(projectileSystemSignature);
	}

	//NpcSystem
	{
		ComponentSignature npcSystemSignature;
		npcSystemSignature.set(myWorld->GetComponentSignatureID<NpcComponent>());
		npcSystemSignature.set(myWorld->GetComponentSignatureID<TransformComponent>());
		myWorld->RegisterSystem<NpcSystem>(npcSystemSignature);
	}

}

void GameScene::InitScripts(const std::string& aLevelName)
{
	myScripts = ScriptManager::GetLevelScripts(aLevelName);

	for (size_t i = 0; i < myScripts.size(); i++)
	{
		myScripts[i]->Init();
	}
}
