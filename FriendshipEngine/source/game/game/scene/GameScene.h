#pragma once
#include "Scene.h"
#include "gui/MenuHandler.h"
#include "../game/game/scene/utility/GameTimer.h"


class GameScene : public Scene
{
public:
	GameScene(SceneManager* aSceneManager);
	~GameScene() override;

	void Init(PhysXSceneManager& aPhysXManager) override;
	bool Update(SceneUpdateContext& aContext) override;
	void Render() override;

	void OnEnter() override;

private:
	void InitComponents() override;
	void InitSystems(PhysXSceneManager& aPhysXManager) override;
	void InitScripts(const std::string& aLevelName) override;

	MENU::MenuHandler myMenuHandler;
	GameTimer myGameTimer;
};