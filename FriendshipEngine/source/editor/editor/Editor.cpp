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
	camera->SetPosition({ 0, 1000, -3000.0f });
	camera->SetRotation({ 25, 0, 0 });

	myEditorManager->Init(myGame);
}

void Editor::Update(const float& dt)
{
	ImGuiHandler::Update();

	EditorUpdateContext context = { dt, myGame->GetSceneManager().GetCurrentWorld()};
	myEditorManager->Update(context);
}

void Editor::Render()
{
	ImGuiHandler::Render();
}
