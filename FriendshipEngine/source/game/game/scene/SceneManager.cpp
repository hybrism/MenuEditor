#include "pch.h"
#include "SceneManager.h"

#include <engine/Defines.h>
#include <engine/Engine.h>
#include <engine/utility/InputManager.h>

#include "Scene.h"
#include "GameScene.h"
#include "MainMenuScene.h"

#include "../factory/ProjectileFactory.h"
#include "../gui/MenuHandler.h"


SceneManager::SceneManager()
{
	//TODO: Add levels here when we have more
	myGameLevels[(int)eLevel::FeatureGym] = "lvl_featureGym.json";
	myGameLevels[(int)eLevel::AssetGym] = "lvl_assetGym.json";
	myGameLevels[(int)eLevel::Lvl3_FamilyHeirloom] = "lvl_familyHeirloom.json";


	myGameLevels[(int)eLevel::AxelFeatureGym] = "lvl_axelFeatureGym.json";

	myScenes[(int)eSceneType::MainMenu] = new MainMenuScene(this);
	myScenes[(int)eSceneType::Game] = new GameScene(this);

	myLoadingScreen = new MENU::MenuHandler();


#ifdef _EDITOR 
	myIsPaused = true;
#else
	myIsPaused = false;
#endif
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

	myLoadingScreen->Init("loadingScreen.json");

#ifdef _EDITOR
	LoadSceneInternal({ eSceneType::Game, "lvl_featureGym.json" });
	//LoadSceneInternal({ eSceneType::Game, "lvl_scriptedEventTestScene.json" });
#endif //!_EDITOR

#ifdef _LAUNCHER
#if START_WITH_MAINMENU
	LoadSceneInternal({ eSceneType::MainMenu, eLevel::None });
#else
	LoadSceneInternal({ eSceneType::Game, eLevel::FeatureGym });
#endif //!START_WITH_MAINMENU
#endif //!_DEBUG

#ifdef _RELEASE
	LoadSceneInternal({ eSceneType::MainMenu, eLevel::None });
#endif
}

bool SceneManager::Update(const SceneUpdateContext& aContext)
{
	if (mySceneStack.empty())
		return false;


//NOTE: This is _LAUNCHER due to there being a different 'R'-command in GameViewWindow for _EDITOR only 
#ifndef _LAUNCHER 
	if (InputManager::GetInstance()->IsKeyPressed('R'))
	{
		ReloadCurrentScene();
}
#endif // _LAUNCHER

	if (!GetCurrentScene()->Update(aContext))
	{
		PopScene();
	}

	return true;
}

void SceneManager::LateUpdate()
{
	if (mySceneToLoad.sceneType == eSceneType::Count)
		return;

	if (!LoadSceneInternal(mySceneToLoad))
		return;

	myCurrentScene = mySceneToLoad;
	GetCurrentScene()->OnEnter();
	mySceneToLoad = {};
}

void SceneManager::Render()
{
	if (mySceneStack.empty())
		return;

	GetCurrentScene()->Render();

	myLoadingScreen->Render();
}

void SceneManager::SetIsPaused(bool aIsPaused)
{
	myIsPaused = aIsPaused;

	auto im = InputManager::GetInstance();
	if (myIsPaused)
		im->UnlockMouseScreen();
	else
		im->LockMouseScreen(Engine::GetInstance()->GetWindowHandle());
}

void SceneManager::TogglePaused()
{
	myIsPaused = !myIsPaused;

	auto im = InputManager::GetInstance();
	if (myIsPaused)
		im->UnlockMouseScreen();
	else
		im->LockMouseScreen(Engine::GetInstance()->GetWindowHandle());
}

void SceneManager::LoadScene(const SceneParameter& aSceneData)
{
	mySceneToLoad = aSceneData;
}

void SceneManager::PopScene()
{
	mySceneStack.pop();

	if (mySceneStack.empty())
		return;

	GetCurrentScene()->OnEnter();
}

void SceneManager::ReloadCurrentScene()
{
	LoadScene(myCurrentScene);
}

bool SceneManager::LoadSceneInternal(const SceneParameter& aData)
{
	if (aData.sceneType == eSceneType::Count)
	{
		PrintE("[SceneManager.cpp] You must specify SceneType in SceneParameter!");
		return false;
	}

	if (mySceneStack.empty() || GetCurrentScene()->GetType() != aData.sceneType)
	{
		mySceneStack.push(myScenes[(int)aData.sceneType]);
	}

	myCurrentScene = aData;

	//TODO: BÖRJAR LADDA EN SCEN I EN TRÅD
	//TODO: FLYTTA TILL EGEN FUNKTION vvv
	if (std::holds_alternative<eLevel>(aData.gameLevel))
	{
		if (std::get<eLevel>(aData.gameLevel) == eLevel::None)
		{
#ifdef _LAUNCHER
			InputManager::GetInstance()->UnlockMouseScreen();
#endif
			return true;
		}

		LoadLevelInternal(myGameLevels[(int)std::get<eLevel>(aData.gameLevel)]);
#ifdef _LAUNCHER
		InputManager::GetInstance()->LockMouseScreen(Engine::GetInstance()->GetWindowHandle());
#endif
		return true;
	}

	if (std::holds_alternative<std::string>(aData.gameLevel))
	{
		LoadLevelInternal(std::get<std::string>(aData.gameLevel));
#ifdef _LAUNCHER
		InputManager::GetInstance()->LockMouseScreen(Engine::GetInstance()->GetWindowHandle());
#endif
		return true;
	}
	// ^^^

	//TODO: VISA LOADINGSCREEN PÅ EN ANNAN TRÅD TILLS SCENEN LADDAT KLART

	return true;
}

void SceneManager::LoadLevelInternal(const std::string& aLevelName)
{
	AssetDatabase::LoadVertexTextures(aLevelName.substr(0, aLevelName.find_last_of(".")));

	myPhysXManager.Init(); //TODO: Namechange?

	myUnityImporter.LoadComponents(aLevelName, GetCurrentWorld(), myPhysXManager);

	GetCurrentScene()->InitScripts(aLevelName);

	//TODO: Move this or rename functions
	ProjectileFactory::GetInstance().Init(GetCurrentWorld(), 29, myPhysXManager);

	GetCurrentScene()->myWorld->InitSystems();
}

bool SceneManager::GetIsPaused() const
{
	return myIsPaused;
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

std::string SceneManager::GetCurrentSceneName()
{
	if (std::holds_alternative<eLevel>(myCurrentScene.gameLevel))
		return myGameLevels[(int)std::get<eLevel>(myCurrentScene.gameLevel)];;

	if (std::holds_alternative<std::string>(myCurrentScene.gameLevel))
		return std::get<std::string>(myCurrentScene.gameLevel);

	return "";
}