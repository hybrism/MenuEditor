#pragma once
#include "SceneCommon.h"
#include "../scripting/ScriptRuntimeInstance.h"

class World;
class PhysXSceneManager;

class Scene
{
	friend class SceneManager;
	friend class UnityImporter;

public:
	Scene() = delete;
	Scene(SceneManager* aSceneManager)
		: mySceneManager(aSceneManager)
	{}
	virtual ~Scene()
	{
		myScripts.clear();
		mySceneManager = nullptr;
	}

	virtual void Init(PhysXSceneManager& aPhysXManager) { aPhysXManager; }
	virtual bool Update(SceneUpdateContext& dt) { dt; return true; }
	virtual void Render() { __noop; }

	virtual void OnEnter() { __noop; }

	eSceneType GetType() const { return myType; }
	eLevel GetLevel() const { return myLevel; }

protected:
	eSceneType myType = eSceneType::Count;
	eLevel myLevel = eLevel::Count;

	World* myWorld = nullptr;
	SceneManager* mySceneManager = nullptr;
	
	std::vector<std::unique_ptr<ScriptRuntimeInstance>> myScripts;

	virtual void InitComponents() { __noop; };
	virtual void InitSystems(PhysXSceneManager& aPhysXManager) { aPhysXManager; }
	virtual void InitScripts(const std::string& aLevelName) { aLevelName; };
};
