#pragma once
#include "Scene.h"

class GameScene : public Scene
{
public:
	GameScene();

	void Init(PhysXSceneManager& aPhysXManager) override;
	bool Update(float dt) override;
	void Render() override;

private:
	void InitComponents() override;
	void InitSystems(PhysXSceneManager& aPhysXManager) override;

protected:
	void InitScripts(const std::string& aLevelName) override;
};