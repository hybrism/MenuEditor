#pragma once
#include "Scene.h"
#include "gui/MenuHandler.h"

class MainMenuScene : public Scene
{
public:
	MainMenuScene(SceneManager* aSceneManager);

	void Init(PhysXSceneManager& aPhysXManager) override;
	bool Update(const SceneUpdateContext& aContext) override;
	void Render() override;

	void OnEnter() override;

private:
	void InitComponents() override;
	void InitSystems(PhysXSceneManager& aPhysXManager) override;

	MENU::MenuHandler myMenuHandler;

};