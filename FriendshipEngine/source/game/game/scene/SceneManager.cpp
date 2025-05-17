#include "pch.h"
#include "SceneManager.h"

#include <engine/Defines.h>
#include <engine/Engine.h>
#include <engine\utility\InputManager.h>

#include "Scene.h"
#include "GameScene.h"
#include "MainMenuScene.h"

#include "../factory/ProjectileFactory.h"
#include "../gui/MenuHandler.h"
#include <engine/graphics/vfx/ParticleSystemManager.h>
#include <engine/graphics/Light/LightManager.h>
#include "audio/NewAudioManager.h"

SceneManager::SceneManager()
{
	//TODO: Add levels here when we have more
	myGameLevels[(int)eLevel::FeatureGym] = "lvl_featureGym";
	myGameLevels[(int)eLevel::Lvl0] = "lvl_0";
	myGameLevels[(int)eLevel::Lvl1] = "lvl_1";
	myGameLevels[(int)eLevel::Lvl2] = "lvl_2";
	myGameLevels[(int)eLevel::Lvl3] = "lvl_3";

	myScenes[(int)eSceneType::MainMenu] = new MainMenuScene(this);
	myScenes[(int)eSceneType::Game] = new GameScene(this);

	//myLoadingScreen = new MENU::MenuHandler();
	//myIsLoading = false;

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

void SceneManager::Init(SceneUpdateContext& aContext)
{
	for (size_t i = 0; i < (int)eSceneType::Count; i++)
	{
		myScenes[i]->Init(myPhysXManager);
	}

	//myLoadingScreen->Init("loadingScreen.json");

#ifdef _EDITOR
//#if START_WITH_MAINMENU
	LoadSceneInternal({ eSceneType::MainMenu, eLevel::None }, aContext);
//#endif //!START_WITH_MAINMENU

	//LoadSceneInternal({ eSceneType::Game, "lvl_featureGym" }, aContext);
#endif //!_EDITOR

#ifdef _LAUNCHER
#if START_WITH_MAINMENU
	LoadSceneInternal({ eSceneType::MainMenu, eLevel::None }, aContext);
#else
	LoadSceneInternal({ eSceneType::Game, eLevel::FeatureGym }, aContext);
#endif //!START_WITH_MAINMENU
#endif //!_DEBUG

#ifdef _RELEASE
	LoadSceneInternal({ eSceneType::MainMenu, eLevel::None }, aContext);
#endif
}

bool SceneManager::Update(SceneUpdateContext& aContext)
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

void SceneManager::LateUpdate(SceneUpdateContext& aContext)
{
	myGameTimer = aContext.gameTimer;

	if (mySceneToLoad.sceneType == eSceneType::Count)
		return;

	if (!LoadSceneInternal(mySceneToLoad, aContext))
		return;

	myCurrentScene = mySceneToLoad;
	GetCurrentScene()->OnEnter();
	mySceneToLoad = {};

}

void SceneManager::Render()
{
	if (mySceneStack.empty())
		return;

	//if (myIsLoading)
	//{
	//	myLoadingScreen->Render();
	//	return;
	//}

	GetCurrentScene()->Render();

}

void SceneManager::SetIsPaused(bool aIsPaused)
{
	myIsPaused = aIsPaused;

	auto im = InputManager::GetInstance();
	if (im == nullptr) { return; }

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

std::string SceneManager::GetLevelNameFromSceneParameter(const SceneParameter& aSceneParam)
{
	if (std::holds_alternative<eLevel>(aSceneParam.gameLevel))
	{
		if (std::get<eLevel>(aSceneParam.gameLevel) == eLevel::None)
		{
			return std::string();
		}

		return myGameLevels[(int)std::get<eLevel>(aSceneParam.gameLevel)];
	}

	if (std::holds_alternative<std::string>(aSceneParam.gameLevel))
	{
		return std::get<std::string>(aSceneParam.gameLevel);
	}

	return std::string();
}

bool SceneManager::LoadSceneInternal(const SceneParameter& aData, SceneUpdateContext& aContext)
{
	if (aData.sceneType == eSceneType::Count)
	{
		PrintE("[SceneManager.cpp] You must specify SceneType in SceneParameter!");
		return false;
	}
	if (mySceneStack.empty() || GetCurrentScene()->GetType() != aData.sceneType)
	{
		mySceneStack.push(myScenes[(int)aData.sceneType]);
		GetCurrentScene()->OnEnter();
	}

	myCurrentScene = aData;

	LoadLevelInternal(GetLevelNameFromSceneParameter(aData), aContext);

	return true;
}

void SceneManager::LoadLevelInternal(const std::string& aLevelName, SceneUpdateContext& aContext)
{
	if (aLevelName.empty())
		return;
	NewAudioManager::GetInstance()->LoadLevelSong(aLevelName);
	std::string levelNameNoExtension = aLevelName.substr(0, aLevelName.find_last_of("."));
	
	//using namespace std::chrono_literals;
	//std::this_thread::sleep_for(1000ms);

	AssetDatabase::LoadVertexTextures(levelNameNoExtension);

	aContext.particleSystemManager->Reset();

	myPhysXManager.Init();

	myUnityImporter.LoadComponents(levelNameNoExtension, GetCurrentWorld(), myPhysXManager, aContext);

	aContext.lightManager->Init();

	auto* scene = GetCurrentScene();
	scene->InitScripts(levelNameNoExtension);

	//TODO: Move this or rename functions
	ProjectileFactory::GetInstance().Init(GetCurrentWorld(), 29, myPhysXManager);

	scene->myWorld->InitSystems();
	scene->myWorld->Update(aContext);
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