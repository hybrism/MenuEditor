#pragma once
#include "Scene.h"
class MainMenuScene : public Scene
{
public:
	MainMenuScene();

	void Init(PhysXSceneManager& aPhysXManager) override;
	bool Update(float dt) override;
	void Render() override;

private:
	void InitComponents() override;
	void InitSystems(PhysXSceneManager& aPhysXManager) override;

};