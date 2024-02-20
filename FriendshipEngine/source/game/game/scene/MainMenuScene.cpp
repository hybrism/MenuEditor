#include "pch.h"
#include "MainMenuScene.h"

#include <engine/utility/InputManager.h>

MainMenuScene::MainMenuScene()
{
	myType = eSceneType::MainMenu;
	myWorld = new World();
}

void MainMenuScene::Init(PhysXSceneManager&)
{
	myWorld->Init();
}

bool MainMenuScene::Update(float)
{
	return !InputManager::GetInstance()->IsKeyPressed(VK_ESCAPE);
}

void MainMenuScene::Render() { __noop; }
void MainMenuScene::InitComponents() { __noop; }
void MainMenuScene::InitSystems(PhysXSceneManager&) { __noop; }
