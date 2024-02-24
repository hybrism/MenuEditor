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
	virtual void Init(PhysXSceneManager& aPhysXManager) { aPhysXManager; }
	virtual bool Update(float dt) { dt; return true; }
	virtual void Render() { __noop; }

	eSceneType GetType() const { return myType; }
	eLevel GetLevel() const { return myLevel; }

protected:
	eSceneType myType = eSceneType::Count;
	eLevel myLevel = eLevel::Count;

	World* myWorld = nullptr;
	
	std::vector<std::unique_ptr<ScriptRuntimeInstance>> myScripts;

	virtual void InitComponents() { __noop; };
	virtual void InitSystems(PhysXSceneManager& aPhysXManager) { aPhysXManager; }
	virtual void InitScripts(const std::string& aLevelName) { aLevelName; };
};
