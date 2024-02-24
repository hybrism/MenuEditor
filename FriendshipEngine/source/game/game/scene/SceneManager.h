#pragma once
#include <stack>
#include <string>
#include <array>
#include <variant>

#include "utility/UnityImporter.h"
#include "../physics/PhysXSceneManager.h"
#include "../scripting/ScriptManager.h"

#include "SceneCommon.h"

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
	bool Update(float dt);
	void LateUpdate();
	void Render();

	void LoadScene(const SceneParameter& aScene);

	World* GetCurrentWorld();
	Scene* GetCurrentScene();
	eLevel GetCurrentLevel();

private:
	UnityImporter myUnityImporter;
	PhysXSceneManager myPhysXManager;
	ScriptManager myScriptManager;

	std::stack<Scene*> mySceneStack;

	std::array<Scene*, (int)eSceneType::Count> myScenes;
	std::array<std::string, (int)eLevel::Count> myGameLevels;
	
	SceneParameter mySceneToLoad;

	void LoadSceneInternal(const SceneParameter& aScene);
	void LoadLevelInternal(const std::string& aSceneName);
};