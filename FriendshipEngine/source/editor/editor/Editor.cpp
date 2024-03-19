#include "Editor.h"
#include <iostream>

// External
#include <imgui/imgui.h>

// Engine
#include <engine/Paths.h>
#include <engine/Engine.h>
#include <engine/graphics/GraphicsEngine.h>
#include <engine/graphics/Camera.h>
#include <assets/ModelFactory.h>
#include <engine/graphics/model/Mesh.h>
#include <assets/AssetDatabase.h>
#include <engine/utility/InputManager.h>
#include <engine/utility/Error.h>
// Game
#include <game/Game.h>

// Internal
#include "friendshipEditor/EditorManager.h"
#include <shared/imguiHandler/ImGuiHandler.h>

Editor::Editor()
{
	myEditorManager = new EditorManager();
}

Editor::~Editor()
{
	myGame = nullptr;
}

void Editor::Init(Game* aGame)
{
	myGame = aGame;

	ImGuiHandler::Init();

	auto camera = GraphicsEngine::GetInstance()->GetCamera();
	camera->GetTransform().SetPosition({ 0, 1000, -3000.0f });
	camera->GetTransform().SetEulerAngles({ 25, 0, 0 });


	myEditorManager->Init(myGame);
}

void Editor::Update(const float& dt)
{
	ImGuiHandler::Update();

	EditorUpdateContext context = { dt, myGame->GetSceneManager().GetCurrentWorld(), myGame };
	myEditorManager->Update(context);

	SwitchToDebugCamera(dt);
}

void Editor::Render()
{
	ImGuiHandler::Render();
}

void Editor::SwitchToDebugCamera(float dt)
{
	auto* ge = GraphicsEngine::GetInstance();

#ifndef _RELEASE
	if (InputManager::GetInstance()->IsKeyPressed(VK_F1))
	{
		if (ge->GetCamera() == &myDebugCamera.myCamera)
		{
			ge->ResetToViewCamera();
			ge->SetUseOfFreeCamera(false);
			Error::DebugPrintInfo("Deacivate Debug Camera");
			InputManager::GetInstance()->LockMouseScreen(Engine::GetInstance()->GetWindowHandle());

		}
		else
		{
			memcpy(&myDebugCamera, ge->GetCamera(), sizeof(Camera));
			ge->ChangeCurrentCamera(&myDebugCamera.myCamera);
			ge->SetUseOfFreeCamera(true);
			Error::DebugPrintInfo("Debug Camera Active");

		}
		return;
	}

	if (ge->GetCamera() == &myDebugCamera.myCamera)
	{
		if (InputManager::GetInstance()->IsKeyHeld(VK_RBUTTON))
		{
			myDebugCamera.Update(dt);
			InputManager::GetInstance()->LockMouseScreen(Engine::GetInstance()->GetWindowHandle());
		}
		else 
		{
			myDebugCamera.SetSkipFrameUpdateBool(true); 
			InputManager::GetInstance()->UnlockMouseScreen();
		}


		return;
	}
#endif
}
