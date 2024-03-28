#include "pch.h"
#include "MainMenuScene.h"
#include "SceneManager.h"

#include <engine/utility/Error.h>
#include <engine/utility/InputManager.h>
#include <engine/graphics/GraphicsEngine.h>

#include "../gui/MenuUpdateContext.h"

MainMenuScene::MainMenuScene(SceneManager* aSceneManager)
	: Scene(aSceneManager)
{
	myType = eSceneType::MainMenu;
	myWorld = new World();
}

void MainMenuScene::Init(PhysXSceneManager&)
{
	myWorld->Init();
	myMenuHandler.Init("mainMenu.json");
}

bool MainMenuScene::Update(const SceneUpdateContext& aContext)
{
	aContext;

	assert(mySceneManager && "SceneManager is nullptr!");

	auto input = InputManager::GetInstance();
	auto renderSize = GraphicsEngine::GetInstance()->DX().GetViewportDimensions();
	Vector2i mousePos = input->GetTentativeMousePosition();

	MENU::MenuUpdateContext context;
	context.sceneManager = mySceneManager;
	context.menuHandler = &myMenuHandler;
	context.renderSize = renderSize;
	context.mousePosition = { (float)mousePos.x, (float)mousePos.y };
	context.mouseDown = input->IsLeftMouseButtonDown();
	myMenuHandler.Update(context);

	return !input->IsKeyPressed(VK_ESCAPE);
}

void MainMenuScene::Render() 
{ 
	GraphicsEngine* ge = GraphicsEngine::GetInstance();

	RenderState renderState;
	renderState.blendState = BlendState::AlphaBlend;
	renderState.depthStencilState = DepthStencilState::ReadOnly;
	ge->SetRenderState(renderState);

	myMenuHandler.Render();
}

void MainMenuScene::OnEnter()
{
	mySceneManager->SetIsPaused(true);
	myMenuHandler.PopToBaseState();
}

void MainMenuScene::InitComponents() { __noop; }
void MainMenuScene::InitSystems(PhysXSceneManager&) { __noop; }
