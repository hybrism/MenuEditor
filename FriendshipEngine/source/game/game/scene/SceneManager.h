#pragma once
#include <stack>
#include <variant>
//#include <thread>

#include "utility/UnityImporter.h"
#include "../physics/PhysXSceneManager.h"
#include "../scripting/ScriptManager.h"

#include "SceneCommon.h"

namespace MENU
{
	class MenuHandler;
}

class Scene;
class GameTimer;

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

	void Init(SceneUpdateContext&);
	bool Update(SceneUpdateContext&);
	void LateUpdate(SceneUpdateContext&);
	void Render();

	void SetIsPaused(bool aIsPaused);
	void TogglePaused();

	void LoadScene(const SceneParameter& aScene);
	void PopScene();
	void ReloadCurrentScene();

	bool GetIsPaused() const;
	World* GetCurrentWorld();
	Scene* GetCurrentScene();
	std::string GetCurrentSceneName();

#ifdef _EDITOR
	VertexIndexCollection& GetVertexPaintedIndexCollection() { return myUnityImporter.GetVertexPaintedIndexCollection(); }
#endif
private:
	//std::thread myLevelLoadingThread;

	UnityImporter myUnityImporter;
	PhysXSceneManager myPhysXManager;

	std::stack<Scene*> mySceneStack;

	std::array<Scene*, (int)eSceneType::Count> myScenes;
	std::array<std::string, (int)eLevel::Count> myGameLevels;

	SceneParameter mySceneToLoad;
	SceneParameter myCurrentScene;

	ScriptManager myScriptManager;
	bool myIsPaused;

	//bool myIsLoading;
	GameTimer* myGameTimer;

	//MENU::MenuHandler* myLoadingScreen;

	bool LoadSceneInternal(const SceneParameter& aScene, SceneUpdateContext&);
	void LoadLevelInternal(const std::string& aSceneName, SceneUpdateContext&);
	std::string GetLevelNameFromSceneParameter(const SceneParameter& aSceneParam);
};