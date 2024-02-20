#pragma once
#include "SceneCommon.h"
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

	virtual void InitComponents() {};
	virtual void InitSystems(PhysXSceneManager& aPhysXManager) { aPhysXManager; }

};
