#include "pch.h"
#include "GameScene.h"

#include <ecs/World.h>
#include <engine/utility/InputManager.h>
#include <engine/Engine.h>
#include <nlohmann/json.hpp>

#include "../utility/JsonUtility.h"
#include "../scripting/ScriptManager.h"
#include "../scripting/ScriptRuntimeInstance.h"
#include "../scripting/ScriptUpdateContext.h"

#include <engine/utility/InputManager.h>
#include <engine/utility/StringHelper.h>

#include "../gui/MenuUpdateContext.h"
#include "SceneManager.h"


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
//#include "../systems/ScriptableEventSystem.h"
#include "../systems/PhysXSystem.h"
#include "../systems/OrbSystem.h"
#include "../systems/DeathZoneSystem.h"

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
#include "../component/DeathZoneComponent.h"
#include "../component/SphereColliderComponent.h"

#include "../component/EventComponent.h"
#include "../component/ScriptableEventComponent.h"
#include "../component/EnemyComponent.h"
#include "../component/CameraComponent.h"
#include "../component/AnimationDataComponent.h"
#include "../component/ProjectileComponent.h"
#include "../component/PhysXComponent.h"
#include "../component/OrbComponent.h"

#include "audio/NewAudioManager.h"

#pragma endregion

GameScene::GameScene(SceneManager* aSceneManager)
	: Scene(aSceneManager)
{
	myType = eSceneType::Game;

	myWorld = new World();
}

GameScene::~GameScene()
{
	delete myWorld;
	myWorld = nullptr;
}

void GameScene::Init(PhysXSceneManager& aPhysXManager)
{
	myWorld->Init();
	myMenuHandler.Init("pauseMenu.json");

	myGameTimer.Init();
	InitComponents();
	InitSystems(aPhysXManager);
}

bool GameScene::Update(SceneUpdateContext& aContext)
{
	assert(mySceneManager && "SceneManager is nullptr!");

	auto* im = InputManager::GetInstance();

	aContext.menuHandler = &myMenuHandler;
	aContext.gameTimer = &myGameTimer;

	
	myGameTimer.Update(aContext);
	
	if (!mySceneManager->GetIsPaused())
	{
		myWorld->Update(aContext);

		for (size_t i = 0; i < myScripts.size(); i++)
		{
			myScripts[i]->Update({ aContext.dt, myWorld });
		}
	}
	else
	{
#ifndef _EDITOR
		auto renderSize = GraphicsEngine::GetInstance()->DX().GetViewportDimensions();
		Vector2i mousePos = im->GetTentativeMousePosition();

		MENU::MenuUpdateContext context;
		context.sceneManager = mySceneManager;
		context.menuHandler = &myMenuHandler;
		context.postProcess = aContext.postProcess;
		context.lightManager = aContext.lightManager;
		context.renderSize = renderSize;
		context.mousePosition = { (float)mousePos.x, renderSize.y - (float)mousePos.y };
		context.mouseDown = im->IsLeftMouseButtonDown();
		context.mouseReleased = im->IsLeftMouseButtonReleased();

		myMenuHandler.Update(context);
#endif // !_EDITOR
	}



#ifdef _DEBUG
	if (im->IsKeyHeld(VK_SHIFT) && im->IsKeyPressed(VK_ESCAPE))
	{
		return false;
	}
#endif // _DEBUG

#ifndef _EDITOR
	if (im->IsKeyPressed(VK_ESCAPE) && !mySceneManager->GetIsPaused())
	{
		mySceneManager->TogglePaused();
	}
#endif // !_EDITOR


	if(Engine::GetInstance()->GetWindowHandle() != GetForegroundWindow() && !mySceneManager->GetIsPaused()) // Checks if current window is in focus!
	{
		mySceneManager->TogglePaused();
	}

	return true;
}

void GameScene::Render()
{
	myWorld->Render();

	if (myType == eSceneType::Game)
	{
		myGameTimer.Render();
	}

#ifndef _EDITOR
	if (mySceneManager->GetIsPaused())
		myMenuHandler.Render();
#endif // !_EDITOR
}

void GameScene::OnEnter()
{
	mySceneManager->SetIsPaused(false);
	myGameTimer.ResetTimer();

	NewAudioManager::GetInstance()->PlayLoadedSong();
}

void GameScene::InitComponents()
{
	myWorld->RegisterComponent<TransformComponent>();
	myWorld->RegisterComponent<MeshComponent>();
	myWorld->RegisterComponent<PlayerComponent>(1);
	myWorld->RegisterComponent<ColliderComponent>();
	myWorld->RegisterComponent<CollisionDataComponent>();
	myWorld->RegisterComponent<EnemyComponent>(32);
	myWorld->RegisterComponent<HitboxComponent>(64);
	myWorld->RegisterComponent<CameraComponent>(8);
	myWorld->RegisterComponent<AnimationDataComponent>(64);
	myWorld->RegisterComponent<PhysXComponent>(128);
	myWorld->RegisterComponent<OrbComponent>(1);
	myWorld->RegisterComponent<DeathZoneComponent>();
	myWorld->RegisterComponent<ProjectileComponent>(32);
	myWorld->RegisterComponent<NpcComponent>(32);
	myWorld->RegisterComponent<EventComponent>(64);
	myWorld->RegisterComponent<ScriptableEventComponent>(64);
	myWorld->RegisterComponent<SphereColliderComponent>();

#ifndef _RELEASE
	myWorld->RegisterComponent<MetaDataComponent>();
#endif
}

void GameScene::InitSystems(PhysXSceneManager& aPhysXManager)
{
	//TODO: Move all of these to respective constructor

	myWorld->RegisterSystem<PlayerSystem, PhysXSceneManager*>(&aPhysXManager);
	myWorld->RegisterSystem<CameraSystem>();
	myWorld->RegisterSystem<CollisionSystem>();
	myWorld->RegisterSystem<EnemySystem, PhysXSceneManager*>(&aPhysXManager);

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
		myWorld->RegisterSystem<EventSystem>(eventSystemSignature)->SetSceneManager(mySceneManager);
	}

	// AnimatorSystem
	{
		ComponentSignature animationSystemSignature;
		animationSystemSignature.set(myWorld->GetComponentSignatureID<AnimationDataComponent>());
		animationSystemSignature.set(myWorld->GetComponentSignatureID<MeshComponent>());
		myWorld->RegisterSystem<AnimatorSystem>(animationSystemSignature);
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
		npcSystemSignature.set(myWorld->GetComponentSignatureID<AnimationDataComponent>());
		myWorld->RegisterSystem<NpcSystem>(npcSystemSignature);
	}
	//DEATHZONE
	{
		myWorld->RegisterSystem<DeathZoneSystem, PhysXSceneManager*>(&aPhysXManager);
	}

	// This system MUST be the last system to be registered due to rendering order
	myWorld->RegisterSystem<RenderingSystem>();
}

void GameScene::InitScripts(const std::string& aLevelName)
{
	myScripts = ScriptManager::GetLevelScripts(aLevelName);

	for (size_t i = 0; i < myScripts.size(); i++)
	{
		myScripts[i]->Init();
	}
}