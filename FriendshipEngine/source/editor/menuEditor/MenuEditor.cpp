#include "MenuEditor.h"
#include <d3d11.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

//ImGui
#include <imgui/imgui.h>
#include <imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <nlohmann/json.hpp>

//Engine
#include <assets/TextureFactory.h>
#include <engine/Paths.h>
#include <engine/utility/Error.h>
#include <engine/utility/StringHelper.h>
#include <engine/utility/InputManager.h>
#include <engine/graphics/Texture.h>
#include <engine/graphics/GraphicsEngine.h>

//Internal
#include <shared/postMaster/PostMaster.h>
#include "UpdateContext.h"
#include "MenuEditorCommon.h"

#include "windows/AssetsWindow.h"
#include "windows/ConsoleWindow.h"
#include "windows/InspectorWindow.h"
#include "windows/MenuObjectHierarchy.h"
#include "windows/MenuViewWindow.h"

#include "gui/MenuObject.h"
#include "gui/components/SpriteComponent.h"
#include "gui/components/Collider2DComponent.h"
#include "gui/components/TextComponent.h"

MENU::MenuEditor::MenuEditor()
	: myFirstFrameSetup(true)
	, mySelectedObjectID(UINT_MAX)
	, myUpGizmoID(500)
	, myRightGizmoID(501)
{
	for (size_t i = 0; i < (int)ePopup::Count; i++)
	{
		myPopups[i] = false;
	}
}

MENU::MenuEditor::~MenuEditor()
{}

void MENU::MenuEditor::Init()
{
	auto ge = GraphicsEngine::GetInstance();

	//TODO:  ViewPort != RenderSize, add get RenderSize to graphicsengine 23.f is the height of menubar?
	Vector2i viewport = ge->GetViewportDimensions();
	myRenderSize = { (float)viewport.x, (float)viewport.y + 23.f };
	myRenderCenter = myRenderSize * 0.5f;

	//SUBSCRIBE TO EVENTS
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::DdsDropped);
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::PushEntityToInspector);
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::UpdateEditorColliders);
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::NewMenuLoaded);

	//CREATE EDITOR WINDOWS
	myWindows[(int)MENU::WindowID::Assets] = std::make_shared<AssetsWindow>("Assets", true, ImGuiWindowFlags_None);
	myWindows[(int)MENU::WindowID::MenuView] = std::make_shared<MenuViewWindow>("MenuView", false, ImGuiWindowFlags_None);
	myWindows[(int)MENU::WindowID::Console] = std::make_shared<ConsoleWindow>("Console", true, ImGuiWindowFlags_None);
	myWindows[(int)MENU::WindowID::Inspector] = std::make_shared<InspectorWindow>("Inspector", true, ImGuiWindowFlags_None);
	myWindows[(int)MENU::WindowID::MenuObjectHierarchy] = std::make_shared<MenuObjectHierarchy>("MenuObjects", true, ImGuiWindowFlags_None);

	//GET SPRITES
	std::filesystem::create_directory(RELATIVE_SPRITE_ASSET_PATH);
	for (const auto& entry : std::filesystem::directory_iterator(RELATIVE_SPRITE_ASSET_PATH))
	{
		auto ext = entry.path().extension();
		if (ext == ".dds")
		{
			myAssets.textures.push_back(TextureFactory::CreateTexture(RELATIVE_SPRITE_ASSET_PATH + entry.path().filename().string(), false));
			myAssets.textureIdToName.push_back(entry.path().filename().string());
			myAssets.textureNameToId[entry.path().filename().string()] = (UINT)myAssets.textures.size() - 1;
		}
	}

	//GET FONTS
	std::filesystem::create_directory(RELATIVE_FONT_ASSET_PATH);
	for (const auto& entry : std::filesystem::directory_iterator(RELATIVE_FONT_ASSET_PATH))
	{
		auto ext = entry.path().extension();
		if (ext == ".ttf")
		{
			myAssets.fontFiles.push_back(entry.path().filename().string());
		}
	}

	//GET MENUFILES
	std::string menuPath = RELATIVE_IMPORT_DATA_PATH + MENU::MENU_PATH;
	std::filesystem::create_directory(menuPath);
	for (const auto& entry : std::filesystem::directory_iterator(menuPath))
	{
		auto ext = entry.path().extension();
		if (ext == ".json")
		{
			myAssets.saveFiles.push_back(entry.path().filename().string());
		}
	}

	myMenuHandler.Init("testMenu.json");
	GenerateEditorColliders();
}

void MENU::MenuEditor::Update(float)
{
	//Dockspace();
	MenuBar();
	Popups();

	UpdateContext updateContext;
	updateContext.assets = myAssets;
	updateContext.menuHandler = &myMenuHandler;

	for (size_t i = 0; i < (int)MENU::WindowID::Count; i++)
	{
		if (!myWindows[i]->myData.isOpen)
			continue;

		ImGui::SetNextWindowBgAlpha(0.7f);
		myWindows[i]->Show(updateContext);
	}

	MenuUpdateContext context;
	ImVec2 mousePos = ImGui::GetMousePos();
	context.mousePosition = { mousePos.x, mousePos.y };
	context.renderSize = myRenderSize;
	context.mousePressed = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

	myMenuHandler.Update(context);
	myEditorObjectManager.Update(context);

	GizmoUpdate();
}

void MENU::MenuEditor::Render()
{
	GraphicsEngine* ge = GraphicsEngine::GetInstance();
	DebugRenderer& debugRenderer = ge->GetDebugRenderer();

	RenderState renderState;
	renderState.blendState = BlendState::AlphaBlend;
	renderState.depthStencilState = DepthStencilState::ReadOnly;
	ge->SetRenderState(renderState);

	myMenuHandler.Render();
	myEditorObjectManager.Render();

	debugRenderer.Render();
}

void MENU::MenuEditor::GizmoUpdate()
{
	for (unsigned int i = 0; i < myEditorObjectManager.myObjects.size(); i++)
	{
		MenuObject& mo = myEditorObjectManager.GetObjectFromID(myEditorObjectManager.myObjects[i]->GetID());
		if (mo.IsPressed())
		{
			mySelectedObjectID = myMenuHandler.GetObjectFromID(myEditorIDToMenuIDMap[mo.GetID()]).GetID();
			FE::PostMaster::GetInstance()->SendMessage({ FE::eMessageType::PushEntityToInspector, mySelectedObjectID });
		}
	}

	if (mySelectedObjectID != UINT_MAX)
	{
		MenuObject& gizmoUp = myEditorObjectManager.GetObjectFromID(myUpGizmoID);
		MenuObject& gizmoRight = myEditorObjectManager.GetObjectFromID(myRightGizmoID);
		MenuObject& selectedObject = myMenuHandler.myObjectManager.GetObjectFromID(mySelectedObjectID);

		gizmoUp.SetPosition(selectedObject.GetPosition());
		gizmoRight.SetPosition(selectedObject.GetPosition());
	}
}

void MENU::MenuEditor::GenerateEditorColliders()
{
	myEditorObjectManager.ClearAll();
	myEditorIDToMenuIDMap.clear();
	myMenuIDToEditorIDMap.clear();

	//CREATE GIZMO
	{
		MenuObject& mo = myEditorObjectManager.CreateNew(myUpGizmoID, myRenderCenter);
		SpriteComponent& sprite = mo.AddComponent<SpriteComponent>();
		int textureID = myAssets.textureNameToId["s_gizmo_up.dds"];
		sprite.SetTexture(myAssets.textures[textureID], myAssets.textureIdToName[textureID], TextureState::Default);
		sprite.SetColor({ 1.f, 1.f, 1.f, 0.3f }, TextureState::Default);
		sprite.SetTexture(myAssets.textures[textureID], myAssets.textureIdToName[textureID], TextureState::Hovered);
		sprite.SetColor({ 1.f, 1.f, 1.f, 1.f }, TextureState::Hovered);
		sprite.SetTexture(myAssets.textures[textureID], myAssets.textureIdToName[textureID], TextureState::Pressed);
		sprite.SetColor({ 1.f, 1.f, 1.f, 1.f }, TextureState::Pressed);

		Collider2DComponent& collider = mo.AddComponent<Collider2DComponent>();
		collider.SetSize({ 16.f, 32.f });
		collider.SetPosition({ 0.f, 22.f });
		collider.SetShouldRenderColliders(false);
	}

	{
		MenuObject& mo = myEditorObjectManager.CreateNew(myRightGizmoID, myRenderCenter);

		SpriteComponent& sprite = mo.AddComponent<SpriteComponent>();
		int textureID = myAssets.textureNameToId["s_gizmo_right.dds"];

		sprite.SetTexture(myAssets.textures[textureID], myAssets.textureIdToName[textureID], TextureState::Default);
		sprite.SetColor({ 1.f, 1.f, 1.f, 0.3f }, TextureState::Default);
		sprite.SetTexture(myAssets.textures[textureID], myAssets.textureIdToName[textureID], TextureState::Hovered);
		sprite.SetColor({ 1.f, 1.f, 1.f, 1.f }, TextureState::Hovered);
		sprite.SetTexture(myAssets.textures[textureID], myAssets.textureIdToName[textureID], TextureState::Pressed);
		sprite.SetColor({ 1.f, 1.f, 1.f, 1.f }, TextureState::Pressed);

		Collider2DComponent& collider = mo.AddComponent<Collider2DComponent>();
		collider.SetSize({ 32.f, 16.f });
		collider.SetPosition({ 22.f, 0.f });
		collider.SetShouldRenderColliders(false);
	}

	if (myMenuHandler.GetAllStates().empty())
		return;

	MenuState& currentState = myMenuHandler.GetCurrentState();
	for (unsigned int i = 0; i < currentState.objectIds.size(); i++)
	{
		MenuObject& menuMo = myMenuHandler.GetObjectFromID(currentState.objectIds[i]);
		MenuObject& editorMo = myEditorObjectManager.CreateNew();

		myEditorIDToMenuIDMap[editorMo.GetID()] = menuMo.GetID();
		myMenuIDToEditorIDMap[menuMo.GetID()] = editorMo.GetID();
		editorMo.SetPosition(menuMo.GetPosition());
		Collider2DComponent& collider = editorMo.AddComponent<Collider2DComponent>();
		collider.SetShouldRenderColliders(true);
		collider.SetSize({ 150.f, 80.f });
	}
}

void MENU::MenuEditor::Dockspace()
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

	ImGuiID dockspaceID = ImGui::GetID(myWindows[(int)MENU::WindowID::Assets]->myData.handle.c_str());
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

		ImGui::DockBuilderDockWindow(myWindows[(int)MENU::WindowID::MenuView]->myData.handle.c_str(), topLeftAreaID);
		ImGui::DockBuilderDockWindow(myWindows[(int)MENU::WindowID::MenuObjectHierarchy]->myData.handle.c_str(), topRightTopAreaID);
		ImGui::DockBuilderDockWindow(myWindows[(int)MENU::WindowID::Inspector]->myData.handle.c_str(), topRightBottomAreaID);
		ImGui::DockBuilderDockWindow(myWindows[(int)MENU::WindowID::Assets]->myData.handle.c_str(), bottomLeftAreaID);
		ImGui::DockBuilderDockWindow(myWindows[(int)MENU::WindowID::Console]->myData.handle.c_str(), bottomRightAreaID);

		ImGui::DockBuilderFinish(dockspaceID);

		myFirstFrameSetup = true;
	}

}

void MENU::MenuEditor::MenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{

			if (ImGui::BeginMenu("Open"))
			{
				for (size_t i = 0; i < myAssets.saveFiles.size(); i++)
				{
					if (ImGui::MenuItem(myAssets.saveFiles[i].c_str()))
					{
						myMenuHandler.LoadFromJson(myAssets.saveFiles[i]);
						FE::PostMaster::GetInstance()->SendMessage({ FE::eMessageType::NewMenuLoaded, myAssets.saveFiles[i] });
					}
				}
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Save"))
			{
				myMenuHandler.SaveToJson();
			}

			if (ImGui::MenuItem("Save As..."))
			{
				myPopups[(int)ePopup::SaveFileAs] = true;
			}

			if (ImGui::MenuItem("Create New..."))
			{
				myPopups[(int)ePopup::CreateNew] = true;
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
			for (int windowIndex = 0; windowIndex < (int)MENU::WindowID::Count; windowIndex++)
			{
				ImGui::MenuItem(myWindows[windowIndex]->myData.handle.c_str(), NULL, &myWindows[windowIndex]->myData.isOpen);
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void MENU::MenuEditor::Popups()
{
	static std::string newMenuName;

	if (myPopups[(int)ePopup::CreateNew])
	{
		newMenuName = "untitled";
		ImGui::OpenPopup("Create New");
		myPopups[(int)ePopup::CreateNew] = false;
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

			size_t n = newMenuName.find(".json");
			if (n == std::string::npos)
				newMenuName += ".json";

			std::string path = RELATIVE_IMPORT_DATA_PATH + MENU::MENU_PATH + newMenuName;
			nlohmann::json menu;
			std::ofstream dataFile(path);
			dataFile << menu;
			dataFile.close();

			myMenuHandler.Init(newMenuName);
			FE::PostMaster::GetInstance()->SendMessage({FE::eMessageType::NewMenuLoaded, newMenuName});
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

void MENU::MenuEditor::RecieveMessage(const FE::Message& aMessage)
{
	switch (aMessage.myEventType)
	{
	case FE::eMessageType::DdsDropped:
	{
		std::string filename = std::any_cast<std::string>(aMessage.myMessage);
		PrintI(filename);
		break;
	}
	case FE::eMessageType::PushEntityToInspector:
	{
		mySelectedObjectID = std::any_cast<unsigned int>(aMessage.myMessage);
		break;
	}
	case FE::eMessageType::UpdateEditorColliders:
	{
		GenerateEditorColliders();
		mySelectedObjectID = UINT_MAX;
		break;
	}
	case FE::eMessageType::NewMenuLoaded:
	{
		GenerateEditorColliders();
		mySelectedObjectID = UINT_MAX;
		break;
	}
	default:
		break;
	}
}
