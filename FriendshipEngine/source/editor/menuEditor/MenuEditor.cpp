#include "MenuEditor.h"
#include <string>
#include <d3d11.h>
#include <filesystem>

//ImGui
#include <imgui/imgui.h>
#include <imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <nlohmann/json.hpp>

//Engine
#include <engine/utility/Error.h>
#include <engine/utility/StringHelper.h>
#include <engine/graphics/Texture.h>
#include <engine/graphics/GraphicsEngine.h>

//Internal
#include <shared/postMaster/PostMaster.h>

#include "UpdateContext.h"

#include "windows/AssetsWindow.h"
#include "windows/ConsoleWindow.h"
#include "windows/InspectorWindow.h"
#include "windows/MenuObjectHierarchy.h"
#include "windows/MenuViewWindow.h"


ME::MenuEditor::MenuEditor()
	: myFirstFrameSetup(false)
{
	for (size_t i = 0; i < (int)ePopup::Count; i++)
	{
		myPopups[i] = false;
	}
}

ME::MenuEditor::~MenuEditor()
{
}

void ME::MenuEditor::Init()
{
	auto ge = GraphicsEngine::GetInstance();
	Vector2i viewport = ge->GetViewportDimensions();
	myRenderSize = { (float)viewport.x, (float)viewport.y };
	myRenderCenter = myRenderSize * 0.5f;

	//SUBSCRIBE TO EVENTS
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::DdsDropped);

	//CREATE EDITOR WINDOWS
	myWindows[(int)ME::ID::Assets] = std::make_shared<AssetsWindow>("Assets", true, ImGuiWindowFlags_None);
	myWindows[(int)ME::ID::MenuView] = std::make_shared<MenuViewWindow>("MenuView", true, ImGuiWindowFlags_None);
	myWindows[(int)ME::ID::Console] = std::make_shared<ConsoleWindow>("Console", true, ImGuiWindowFlags_None);
	myWindows[(int)ME::ID::Inspector] = std::make_shared<InspectorWindow>("Inspector", true, ImGuiWindowFlags_None);
	myWindows[(int)ME::ID::MenuObjectHierarchy] = std::make_shared<MenuObjectHierarchy>("MenuObjects", true, ImGuiWindowFlags_None);

	//GET SPRITES
	std::string spriteAssetPath = RELATIVE_MENUEDITOR_ASSETS + SPRITES;
	std::filesystem::create_directory(spriteAssetPath);
	for (const auto& entry : std::filesystem::directory_iterator(spriteAssetPath))
	{
		auto ext = entry.path().extension();
		if (ext == ".dds")
		{
			myAssets.textures.push_back(myTextureFactory.CreateTexture(spriteAssetPath + entry.path().filename().string(), false));
		}
	}

	//GET FONTS
	std::string fontAssetPath = RELATIVE_MENUEDITOR_ASSETS + FONT_PATH;
	std::filesystem::create_directory(fontAssetPath);
	for (const auto& entry : std::filesystem::directory_iterator(fontAssetPath))
	{
		auto ext = entry.path().extension();
		if (ext == ".ttf")
		{
			myAssets.fontFiles.push_back(entry.path().filename().string());
		}
	}

	//GET MENUFILES
	std::string menuPath = RELATIVE_MENUEDITOR_ASSETS + MENU_PATH;
	std::filesystem::create_directory(menuPath);
	for (const auto& entry : std::filesystem::directory_iterator(menuPath))
	{
		auto ext = entry.path().extension();
		if (ext == ".json")
		{
			myAssets.saveFiles.push_back(entry.path().filename().string());
		}
	}

	myMenuHandler.Init("testMenu");
}

void ME::MenuEditor::Update(float)
{
	Dockspace();
	Popups();

	UpdateContext updateContext;
	updateContext.textures = myAssets.textures;
	updateContext.menuHandler = &myMenuHandler;

	for (size_t i = 0; i < (int)ME::ID::Count; i++)
	{
		if (!myWindows[i]->myData.isOpen)
			continue;

		myWindows[i]->Show(updateContext);
	}

	myMenuHandler.Update();
}

void ME::MenuEditor::Render()
{
	myMenuHandler.Render();
}

void ME::MenuEditor::Dockspace()
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
	ImGui::Begin("MenuEditorDockspace", NULL, hostWindowFlags);
	ImGui::PopStyleVar(3);
	ImGuiID dockspaceID = ImGui::GetID("MenuEditorDockspace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

	MenuBar();

	ImGui::End();

	if (!myFirstFrameSetup)
	{
		ImGui::DockBuilderRemoveNode(dockspaceID);
		ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

		//		SETUP:
		//							80%		
		//		|-------------------|----|
		//		|					|____|	70%	
		//		|					|	 |
		//		|					|	 |
		//		|___________________|____|
		//		|						 |	15% (height)
		//		|________________________|

		constexpr float ratioHorizontalSplit = 0.15f;
		constexpr float ratioTopWindowSplit = 0.80f;
		constexpr float ratioBottomWindowSplit = 0.50f;
		constexpr float ratioTopRightWindowSplit = 0.70f;

		//Horizontal split
		ImGuiID topAreaID = 0;
		ImGuiID bottomAreaID = 0;
		ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Down, ratioHorizontalSplit, &bottomAreaID, &topAreaID);

		//Split Top area into 2
		ImGuiID topLeftAreaID = 0;
		ImGuiID topRightAreaID = 0;
		ImGui::DockBuilderSplitNode(topAreaID, ImGuiDir_Left, ratioTopWindowSplit, &topLeftAreaID, &topRightAreaID);

		//Split Bottom area into 2
		ImGuiID bottomLeftAreaID = 0;
		ImGuiID bottomRightAreaID = 0;
		ImGui::DockBuilderSplitNode(bottomAreaID, ImGuiDir_Left, ratioBottomWindowSplit, &bottomLeftAreaID, &bottomRightAreaID);

		//Split top right area horiszontally
		ImGuiID topRightTopAreaID = 0;
		ImGuiID topRightBottomAreaID = 0;
		ImGui::DockBuilderSplitNode(topRightAreaID, ImGuiDir_Down, ratioTopRightWindowSplit, &topRightBottomAreaID, &topRightTopAreaID);


		ImGui::DockBuilderDockWindow(myWindows[(int)ME::ID::MenuView]->myData.handle.c_str(), topLeftAreaID);
		ImGui::DockBuilderDockWindow(myWindows[(int)ME::ID::MenuObjectHierarchy]->myData.handle.c_str(), topRightTopAreaID);
		ImGui::DockBuilderDockWindow(myWindows[(int)ME::ID::Inspector]->myData.handle.c_str(), topRightBottomAreaID);
		ImGui::DockBuilderDockWindow(myWindows[(int)ME::ID::Assets]->myData.handle.c_str(), bottomLeftAreaID);
		ImGui::DockBuilderDockWindow(myWindows[(int)ME::ID::Console]->myData.handle.c_str(), bottomRightAreaID);

		ImGui::DockBuilderFinish(dockspaceID);

		myFirstFrameSetup = true;
	}

}

void ME::MenuEditor::MenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Create New..."))
			{
				myPopups[(int)ePopup::CreateNew] = true;
			}

			if (ImGui::MenuItem("Save..."))
			{
				
			}

			if (ImGui::MenuItem("Save As..."))
			{
				myPopups[(int)ePopup::SaveFileAs] = true;
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Exit"))
			{
				PostQuitMessage(0);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows"))
		{
			for (int windowIndex = 0; windowIndex < (int)ME::ID::Count; windowIndex++)
			{
				ImGui::MenuItem(myWindows[windowIndex]->myData.handle.c_str(), NULL, &myWindows[windowIndex]->myData.isOpen);
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void ME::MenuEditor::Popups()
{
	static std::string newMenuName;

	if (myPopups[(int)ePopup::CreateNew])
	{
		newMenuName = "untitled";
		ImGui::OpenPopup("Create New");
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();

	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Create New", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Enter Menu name: ");
		ImGui::InputText("##", &newMenuName, ImGuiInputTextFlags_AutoSelectAll);
	
		if (ImGui::Button("Create", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			myPopups[(int)ePopup::CreateNew] = false;

			//TODO: Add functionality to create new JSON
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			myPopups[(int)ePopup::CreateNew] = false;
		}

		ImGui::EndPopup();
	}
}

void ME::MenuEditor::SaveFile()
{
	std::string basePath = RELATIVE_MENUEDITOR_ASSETS + MENU_PATH;
	basePath;

	//std::filesystem::path menuPath = std::filesystem::path(basePath) / std::filesystem::path(myActiveMenu).replace_extension(".json");

	//nlohmann::json menuData;

	//script.WriteToJson(menuData);

	//std::ofstream out(menuPath);
	//out << menuData.json;
	//out.close();

}

void ME::MenuEditor::RecieveMessage(const FE::Message& aMessage)
{
	switch (aMessage.myEventType)
	{
	case FE::eMessageType::DdsDropped:
	{
		std::string filename = std::any_cast<std::string>(aMessage.myMessage);
		PrintI(filename);
		break;
	}
	default:
		break;
	}
}
