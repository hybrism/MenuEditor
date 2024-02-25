#include "pch.h"
#include "SceneManager.h"

#include <engine/Defines.h>

#include "Scene.h"
#include "GameScene.h"
#include "MainMenuScene.h"

#include "factory/EventFactory.h"
#include "factory/ProjectileFactory.h"

#include <engine/utility/InputManager.h>

SceneManager::SceneManager()
{
	//TODO: Add levels here when we have more
	myGameLevels[(int)eLevel::Lvl3_FamilyHeirloom] = "lvl_familyHeirloom.json";
	myGameLevels[(int)eLevel::AxelFeatureGym] = "lvl_familyHeirloom.json";
	myGameLevels[(int)eLevel::FeatureGym] = "FeatureGym.json";
	myGameLevels[(int)eLevel::AssetGym] = "lvl_assetGym.json";

	myScenes[(int)eSceneType::MainMenu] = new MainMenuScene();
	myScenes[(int)eSceneType::Game] = new GameScene();
}

SceneManager::~SceneManager()
{
	for (size_t i = 0; i < (int)eSceneType::Count; i++)
	{
		delete myScenes[i];
		myScenes[i] = nullptr;
	}
}

void SceneManager::Init()
{
	for (size_t i = 0; i < (int)eSceneType::Count; i++)
	{
		myScenes[i]->Init(myPhysXManager);
	}

#ifdef _DEBUG
#if START_WITH_MAINMENU
	LoadSceneInternal({ eSceneType::MainMenu });
#endif //START_WITH_MAINMENU END
	LoadSceneInternal({ eSceneType::Game, eLevel::FeatureGym });
#else // _DEBUG
	//LoadSceneInternal({ eSceneType::MainMenu });

	LoadSceneInternal({ eSceneType::Game, "lvl_familyHeirloom.json" });

#endif
}

bool SceneManager::Update(float dt)
{
	if (mySceneStack.empty())
		return false;

	if (InputManager::GetInstance()->IsKeyPressed('R'))
		LoadSceneInternal({ eSceneType::Game, eLevel::AxelFeatureGym });

	if (!GetCurrentScene()->Update(dt))
	{
		mySceneStack.pop();
	}

	return true;
}

void SceneManager::LateUpdate()
{
	if (mySceneToLoad.sceneType == eSceneType::Count)
		return;

	LoadSceneInternal(mySceneToLoad);
	mySceneToLoad = {};

}

void SceneManager::Render()
{
	if (mySceneStack.empty())
		return;

	GetCurrentScene()->Render();

}

void SceneManager::LoadScene(const SceneParameter& aSceneData)
{
	mySceneToLoad = aSceneData;
}

void SceneManager::LoadSceneInternal(const SceneParameter& aData)
{
	if (aData.sceneType == eSceneType::Count)
	{
		PrintE("[SceneManager.cpp] You must specify SceneType in SceneParameter!");
		return;
	}

	if (mySceneStack.empty() || GetCurrentScene()->GetType() != aData.sceneType)
	{
		mySceneStack.push(myScenes[(int)aData.sceneType]);
	}


	//TODO: BÖRJAR LADDA EN SCEN I EN TRÅD
	//TODO: FLYTTA TILL EGEN FUNKTION vvv
	if (std::holds_alternative<eLevel>(aData.gameLevel))
	{
		LoadLevelInternal(myGameLevels[(int)std::get<eLevel>(aData.gameLevel)]);
		return;
	}

	if (std::holds_alternative<std::string>(aData.gameLevel))
	{
		LoadLevelInternal(std::get<std::string>(aData.gameLevel));
		return;
	}
	// ^^^

	//TODO: VISA LOADINGSCREEN PÅ EN ANNAN TRÅD TILLS SCENEN LADDAT KLART
}

void SceneManager::LoadLevelInternal(const std::string& aLevelName)
{
	myPhysXManager.Init(); //TODO: Namechange?

	myUnityImporter.LoadComponents(aLevelName, GetCurrentWorld(), myPhysXManager);

	GetCurrentScene()->InitScripts(aLevelName);

	//TODO: Move this or rename functions
	ProjectileFactory::GetInstance().Init(GetCurrentWorld(), 29, myPhysXManager);
	EventFactory::GetInstance().Init(mySceneStack.top()->myWorld);

	GetCurrentScene()->myWorld->InitSystems();
}

Scene* SceneManager::GetCurrentScene()
{
	if (!mySceneStack.empty())
		return mySceneStack.top();

	PrintE("[SceneManager.cpp] SceneStack is empty!");
	return nullptr;
}

eLevel SceneManager::GetCurrentLevel()
{
	if (!mySceneStack.empty())
		return mySceneStack.top()->GetLevel();

	PrintE("[SceneManager.cpp] SceneStack is empty!");
	return eLevel::None;
}

World* SceneManager::GetCurrentWorld()
{
	if (!mySceneStack.empty())
		return mySceneStack.top()->myWorld;

	PrintE("[SceneManager.cpp] SceneStack is empty!");
	return nullptr;
}
