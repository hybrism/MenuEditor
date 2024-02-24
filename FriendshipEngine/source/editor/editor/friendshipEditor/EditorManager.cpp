#include "EditorManager.h"
#include <filesystem>

//Engine
#include <engine/Paths.h>
#include <engine/utility/Error.h>
#include <engine/graphics/GraphicsEngine.h>
#include <engine/graphics/Camera.h>

//External
#include <imgui/imgui_internal.h>
#include <imnodes/imnodes.h>

//Game
#include <game/Game.h>

//Internal
#include "windows/WindowFactory.h"

EditorManager::EditorManager()
	: myGame(nullptr)
	, myFirstFrameSetup(false)
{}

EditorManager::~EditorManager()
{}

void EditorManager::Init(Game* aGame)
{
	myGame = aGame;

	LoadImportFiles();

	//myMenuEditor.Init(myGame);

	for (size_t i = 0; i < (int)FE::ID::Count; i++)
	{
		myWindows[(FE::ID)i] = FE::WindowFactory::Create((FE::ID)i);
	}
}

void EditorManager::Update(EditorUpdateContext aContext)
{
	Dockspace();

	aContext.editorManager = this;

	for (size_t i = 0; i < (int)FE::ID::Count; i++)
	{
		if (!myWindows[(FE::ID)i]->myData.isOpen)
			continue;

		myWindows[(FE::ID)i]->Show(aContext);
	}

	ShowDebugData(aContext.dt);

}

void EditorManager::ShowDebugData(const float& dt)
{
	auto* ge = GraphicsEngine::GetInstance();

	myFPSTimer += dt;
	if (myFPSTimer >= myFPSUpdateFrequency)
	{
		myFPS = static_cast<int>(1.f / dt);
		myFPSTimer = 0.f;
	}

	if (ImGui::Begin(myWindows[FE::ID::GameView]->myData.handle.c_str()))
	{
		ImVec2 wPos = ImGui::GetWindowPos();
		ImVec2 wMin = ImGui::GetWindowContentRegionMin();
		ImVec2 topLeft = wPos + wMin;

		const ImVec2 textPos = ImVec2(topLeft.x + 10, topLeft.y + 10);
		ImVec2 textOffset = ImVec2(0, 15);

		std::string fps = "FPS: " + std::to_string(myFPS);
		std::string drawCall = "Draw Calls: " + std::to_string(ge->GetDrawCalls());

		ImDrawList* tDrawList = ImGui::GetWindowDrawList();

		tDrawList->AddText(textPos, IM_COL32_WHITE, fps.c_str());
		tDrawList->AddText(textPos + textOffset, IM_COL32_WHITE, drawCall.c_str());
	}
	ImGui::End();
}

void EditorManager::Dockspace()
{
	const auto& viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGuiWindowFlags hostWindowFlags = 0;
	hostWindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
	hostWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	hostWindowFlags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("MainDockspace", NULL, hostWindowFlags);
	ImGui::PopStyleVar(3);
	ImGuiID dockspaceID = ImGui::GetID("Dockspace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

	MenuBar();

	ImGui::End();

	if (!myFirstFrameSetup)
	{
		ImGui::DockBuilderRemoveNode(dockspaceID);
		ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

		//		SETUP:
		//	(width)	20%				80%		
		//		|----|--------------|----|
		//		|	 |				|	 |	
		//		|	 |				|	 |
		//		|	 |				|	 |
		//		|____|______________|____|
		//		|			70%	|		 |	25% (height)
		//		|_______________|________|

		constexpr float ratioHorizontalSplit = 0.25f;
		constexpr float ratioFirstTopWindowSplit = 0.20f;
		constexpr float ratioSecondTopWindowSplit = 0.80f;
		constexpr float ratioBottomWindowSplit = 0.70f;

		//Horizontal split
		ImGuiID topAreaID = 0;
		ImGuiID bottomAreaID = 0;
		ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Down, ratioHorizontalSplit, &bottomAreaID, &topAreaID);

		//Split Bottom area into 2
		ImGuiID assetDatabaseID = 0;
		ImGuiID consoleID = 0;
		ImGui::DockBuilderSplitNode(bottomAreaID, ImGuiDir_Left, ratioBottomWindowSplit, &assetDatabaseID, &consoleID);

		//Split Top area into 3
		ImGuiID topRightAreaID = 0;
		ImGuiID sceneHierarchyID = 0;
		ImGuiID inspectorID = 0;
		ImGuiID gameviewID = 0;
		ImGuiID vertexPainterID = 0;

		ImGui::DockBuilderSplitNode(topAreaID, ImGuiDir_Left, ratioFirstTopWindowSplit, &sceneHierarchyID, &topRightAreaID);
		ImGui::DockBuilderSplitNode(topRightAreaID, ImGuiDir_Left, ratioSecondTopWindowSplit, &gameviewID, &inspectorID);

		ImGui::DockBuilderDockWindow(myWindows[FE::ID::SceneHierarchy]->myData.handle.c_str(), sceneHierarchyID);
		ImGui::DockBuilderDockWindow(myWindows[FE::ID::Inspector]->myData.handle.c_str(), inspectorID);
		ImGui::DockBuilderDockWindow(myWindows[FE::ID::GameView]->myData.handle.c_str(), gameviewID);
		ImGui::DockBuilderDockWindow(myWindows[FE::ID::EditorView]->myData.handle.c_str(), gameviewID);
		ImGui::DockBuilderDockWindow(myWindows[FE::ID::AssetDatabase]->myData.handle.c_str(), assetDatabaseID);
		ImGui::DockBuilderDockWindow(myWindows[FE::ID::Console]->myData.handle.c_str(), consoleID);
		ImGui::DockBuilderDockWindow(myWindows[FE::ID::VertexPainter]->myData.handle.c_str(), vertexPainterID);

		ImGui::DockBuilderFinish(dockspaceID);

		//MakeTabVisible(myWindows[FE::ID::GameView]->myData.handle.c_str());

		myFirstFrameSetup = true;
	}
}

void EditorManager::MenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit", "Shift+Esc"))
			{
				PostQuitMessage(0);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows"))
		{
			for (int windowIndex = 0; windowIndex < (int)FE::ID::Count; windowIndex++)
			{
				ImGui::MenuItem(myWindows[(FE::ID)windowIndex]->myData.handle.c_str(), NULL, &myWindows[(FE::ID)windowIndex]->myData.isOpen);
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Levels"))
		{
			for (std::string file : myJsonFileNames)
			{
				if (ImGui::MenuItem(file.c_str()))
				{
					myGame->GetSceneManager().LoadScene({ eSceneType::Game, file });
				}
			}

			if (ImGui::MenuItem("[Update files...]"))
				LoadImportFiles();

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void EditorManager::LoadImportFiles()
{
	myJsonFileNames.clear();

	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(RELATIVE_IMPORT_PATH))
	{
		std::string file = entry.path().filename().string();
		if ((file != "Resources.json") && (file != "Resources.friend"))
		{
			if (entry.path().filename().extension() == ".json")
			{
				myJsonFileNames.emplace_back(file);
			}
		}
	}
}

void EditorManager::MakeTabVisible(const char* window_name)
{
	ImGuiWindow* window = ImGui::FindWindowByName(window_name);
	if (window == NULL || window->DockNode == NULL || window->DockNode->TabBar == NULL)
		return;

	window->DockNode->TabBar->NextSelectedTabId = window->TabId;
}