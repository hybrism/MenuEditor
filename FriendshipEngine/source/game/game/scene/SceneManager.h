#pragma once
#include <stack>
#include <string>
#include <array>
#include <variant>

#include "utility/UnityImporter.h"
#include "../physics/PhysXSceneManager.h"
#include "../scripting/ScriptManager.h"

#include "SceneCommon.h"

namespace MENU
{
	class MenuHandler;
}

class Scene;

struct SceneParameter
{
	eSceneType sceneType = eSceneType::Count;
	std::variant<eLevel, std::string> gameLevel = {};
};

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	void Init();
	bool Update(const SceneUpdateContext& dt);
	void LateUpdate();
	void Render();

	void SetIsPaused(bool aIsPaused);
	void TogglePaused();

	void LoadScene(const SceneParameter& aScene);
	void PopScene();
	void ReloadCurrentScene();

	bool GetIsPaused() const;
	World* GetCurrentWorld();
	Scene* GetCurrentScene();
	eLevel GetCurrentLevel();
	std::string GetCurrentSceneName();

private:
	UnityImporter myUnityImporter;
	PhysXSceneManager myPhysXManager;

	std::stack<Scene*> mySceneStack;

	std::array<Scene*, (int)eSceneType::Count> myScenes;
	std::array<std::string, (int)eLevel::Count> myGameLevels;

	SceneParameter mySceneToLoad;
	SceneParameter myCurrentScene;

	ScriptManager myScriptManager;
	bool myIsPaused;

	MENU::MenuHandler* myLoadingScreen;

	bool LoadSceneInternal(const SceneParameter& aScene);
	void LoadLevelInternal(const std::string& aSceneName);
};