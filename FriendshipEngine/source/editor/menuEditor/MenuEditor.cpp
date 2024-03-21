#include "MenuEditor.h"

#include <d3d11.h>
#include <filesystem>
#include <fstream>

//External
#include <imgui/imgui.h>
#include <imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <nlohmann/json.hpp>

//Engine
#include <engine/Paths.h>
#include <engine/utility/Error.h>
#include <assets/TextureFactory.h>
#include <engine/graphics/GraphicsEngine.h>

//Internal
#include <shared/postMaster/PostMaster.h>
#include "windows/AssetsWindow.h"
#include "windows/ConsoleWindow.h"
#include "windows/InspectorWindow.h"
#include "windows/ObjectHierarchyWindow.h"
#include "windows/MenuViewWindow.h"

//Game
#include <game/gui/MenuObject.h>
#include <game/gui/components/SpriteComponent.h>
#include <game/gui/components/Collider2DComponent.h>
#include <game/gui/components/TextComponent.h>

MENU::MenuEditor::MenuEditor()
	: myFirstFrameSetup(false)
	, myShouldShowDebugData(false)
	, myShouldShowEditorColliders(false)
	, myShouldShowMenuColldiers(true)
	, mySelectedObjectID(INVALID_ID)
	, myEditorIDStartIndex(400)
	, myGizmoID(INVALID_ID)
{}

MENU::MenuEditor::~MenuEditor()
{}

void MENU::MenuEditor::Init()
{
	auto ge = GraphicsEngine::GetInstance();

	//TODO:  ViewPort != RenderSize, add get RenderSize to graphicsengine 23.f is the height of menubar?
	Vector2i viewport = ge->DX().GetViewportDimensions();
	myRenderSize = { (float)viewport.x, (float)viewport.y + 23.f };
	myRenderCenter = myRenderSize * 0.5f;

	////SUBSCRIBE TO EVENTS
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::DdsDropped);
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::TtfDropped);
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::PushEntityToInspector);
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::UpdateMenuEditorColliders);
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::NewMenuLoaded);

	////CREATE EDITOR WINDOWS
	myWindows[(int)MENU::WindowID::Assets] = std::make_shared<AssetsWindow>("Assets", false, ImGuiWindowFlags_None);
	myWindows[(int)MENU::WindowID::MenuView] = std::make_shared<MenuViewWindow>("MenuView", false, ImGuiWindowFlags_NoBackground);
	myWindows[(int)MENU::WindowID::Console] = std::make_shared<ConsoleWindow>("Console", false, ImGuiWindowFlags_None);
	myWindows[(int)MENU::WindowID::Inspector] = std::make_shared<InspectorWindow>("Inspector", true, ImGuiWindowFlags_None);
	myWindows[(int)MENU::WindowID::MenuObjectHierarchy] = std::make_shared<MenuObjectHierarchy>("MenuObjects", true, ImGuiWindowFlags_None);

	////GET SPRITES
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

	////GET FONTS
	std::filesystem::create_directory(RELATIVE_FONT_ASSET_PATH);
	for (const auto& entry : std::filesystem::directory_iterator(RELATIVE_FONT_ASSET_PATH))
	{
		auto ext = entry.path().extension();
		if (ext == ".ttf")
		{
			myAssets.fontFiles.push_back(entry.path().filename().string());
		}
	}

	////GET MENUFILES
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

	myMenuHandler.Init("mainMenu.json", nullptr);

	GenerateEditorColliders();
}

void MENU::MenuEditor::Update(float)
{
	Dockspace();
	Popups();

	MenuEditorUpdateContext editorUpdateContext;
	editorUpdateContext.assets = myAssets;
	editorUpdateContext.menuHandler = &myMenuHandler;
	editorUpdateContext.showDebugData = myShouldShowDebugData;

	for (size_t i = 0; i < (int)MENU::WindowID::Count; i++)
	{
		if (!myWindows[i]->myData.isOpen)
			continue;

		myWindows[i]->Show(editorUpdateContext);
	}

	MenuUpdateContext menuContext;
	ImVec2 mousePos = ImGui::GetMousePos();
	menuContext.mousePosition = { mousePos.x, mousePos.y };
	menuContext.renderSize = myRenderSize;
	menuContext.mousePressed = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

	myMenuHandler.Update(menuContext);

	menuContext.mousePressed = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
	myEditorObjectManager.Update(menuContext);

	GizmoUpdate();
}

void MENU::MenuEditor::Render()
{
	GraphicsEngine* ge = GraphicsEngine::GetInstance();
	DebugRenderer& debugRenderer = ge->GetDebugRenderer();
	SpriteRenderer& spriteRenderer = ge->GetSpriteRenderer();

	RenderState renderState;
	renderState.blendState = BlendState::AlphaBlend;
	renderState.depthStencilState = DepthStencilState::ReadOnly;
	ge->SetRenderState(renderState);

	myMenuHandler.Render();
	myEditorObjectManager.Render();

	spriteRenderer.Render();
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
			break;
		}
	}

	if (mySelectedObjectID != UINT_MAX)
	{
		MenuObject& gizmo = myEditorObjectManager.GetObjectFromID(myGizmoID);
		MenuObject& selectedObject = myMenuHandler.myObjectManager.GetObjectFromID(mySelectedObjectID);

		gizmo.SetPosition(selectedObject.GetPosition());
	}
}

void MENU::MenuEditor::GenerateEditorColliders()
{
	myEditorObjectManager.ClearAll();
	myEditorIDToMenuIDMap.clear();
	myMenuIDToEditorIDMap.clear();

	if (myMenuHandler.GetAllStates().empty())
		return;

	ID editorID = myEditorIDStartIndex;

	MenuState& currentState = myMenuHandler.GetCurrentState();
	for (unsigned int i = 0; i < currentState.objectIds.size(); i++)
	{
		MenuObject& menuMo = myMenuHandler.GetObjectFromID(currentState.objectIds[i]);
		MenuObject& editorMo = myEditorObjectManager.CreateNew(editorID++);

		myEditorIDToMenuIDMap[editorMo.GetID()] = menuMo.GetID();
		myMenuIDToEditorIDMap[menuMo.GetID()] = editorMo.GetID();
		editorMo.SetPosition(menuMo.GetPosition());
		Collider2DComponent& collider = editorMo.AddComponent<Collider2DComponent>();
		collider.SetShouldRenderColliders(myShouldShowEditorColliders);
		collider.SetSize({ 100.f, 100.f });
	}

	//CREATE GIZMO
	{
		MenuObject& mo = myEditorObjectManager.CreateNew(editorID++, myRenderCenter);
		myGizmoID = mo.GetID();
		SpriteComponent& sprite = mo.AddComponent<SpriteComponent>();
		int textureID = myAssets.textureNameToId["s_gizmo.dds"];
		sprite.SetTexture(myAssets.textures[textureID], myAssets.textureIdToName[textureID], ObjectState::Default);
		sprite.SetColor({ 1.f, 1.f, 1.f, 0.3f }, ObjectState::Default);
		sprite.SetTexture(myAssets.textures[textureID], myAssets.textureIdToName[textureID], ObjectState::Hovered);
		sprite.SetColor({ 1.f, 1.f, 1.f, 1.f }, ObjectState::Hovered);
		sprite.SetTexture(myAssets.textures[textureID], myAssets.textureIdToName[textureID], ObjectState::Pressed);
		sprite.SetColor({ 1.f, 1.f, 1.f, 1.f }, ObjectState::Pressed);

		Collider2DComponent& collider = mo.AddComponent<Collider2DComponent>();
		collider.SetSize(sprite.GetTextureSize());
		collider.SetShouldRenderColliders(myShouldShowEditorColliders);
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

	ImGuiID dockspaceID = viewport->ID;
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

	MenuBar();

	ImGui::End();

	if (!myFirstFrameSetup)
	{
		//		SETUP:
		//							80%		
		//		|-------------------|----|
		//		|					|	 |		
		//		|					|	 |
		//		|					|----|	60%
		//		|					|	 |
		//		|					|	 |
		//		|___________________|____|

		constexpr float ratioWindowSplit = 0.80f;
		constexpr float ratioTopRightWindowSplit = 0.60f;
		constexpr float menuBarHeight = 18.f;

		ImVec2 wSize = ImVec2(viewport->WorkSize.x * (1 - ratioWindowSplit), viewport->WorkSize.y - menuBarHeight);
		ImVec2 wMin = ImVec2(viewport->WorkSize.x * ratioWindowSplit, viewport->WorkPos.y + menuBarHeight);
		ImVec2 wMax = ImVec2(viewport->WorkSize);


		MENU::WindowData& hierarchyWindow = myWindows[(int)MENU::WindowID::MenuObjectHierarchy]->myData;
		MENU::WindowData& inspectorWindow = myWindows[(int)MENU::WindowID::Inspector]->myData;

		ImGui::SetNextWindowPos(wMin);
		ImGui::SetNextWindowSize(ImVec2(wSize.x, wSize.y * (1 - ratioTopRightWindowSplit)));
		ImGui::Begin(hierarchyWindow.handle.c_str(), &hierarchyWindow.isOpen, hierarchyWindow.flags);
		ImVec2 nextWindowStartPos = ImVec2(wMin.x, wMin.y + ImGui::GetWindowHeight());
		ImGui::End();

		ImGui::SetNextWindowPos(nextWindowStartPos);
		ImGui::SetNextWindowSize(ImVec2(wSize.x, wSize.y * ratioTopRightWindowSplit));
		ImGui::Begin(inspectorWindow.handle.c_str(), &inspectorWindow.isOpen, inspectorWindow.flags);
		ImGui::End();

		myFirstFrameSetup = true;
	}

}

void MENU::MenuEditor::MenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		myContentRegionMin = ImGui::GetWindowContentRegionMin();
		myContentRegionMin.y += ImGui::GetWindowHeight();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::BeginMenu("Open"))
			{
				for (size_t i = 0; i < myAssets.saveFiles.size(); i++)
				{
					if (ImGui::MenuItem(myAssets.saveFiles[i].c_str()))
					{
						myEditorObjectManager.ClearAll();

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

		if (ImGui::BeginMenu("Settings"))
		{
			if (ImGui::MenuItem("Show Editor Colliders", NULL, &myShouldShowEditorColliders))
			{
				for (unsigned int i = 0; i < myEditorObjectManager.myObjects.size(); i++)
				{
					if (!myEditorObjectManager.myObjects[i]->HasComponent<Collider2DComponent>())
						continue;

					Collider2DComponent& collider = myEditorObjectManager.myObjects[i]->GetComponent<Collider2DComponent>();
					collider.SetShouldRenderColliders(myShouldShowEditorColliders);
				}
			}

			if (ImGui::MenuItem("Show Menu Colliders", NULL, &myShouldShowMenuColldiers))
			{
				for (unsigned int i = 0; i < myMenuHandler.myObjectManager.myObjects.size(); i++)
				{
					if (!myMenuHandler.myObjectManager.myObjects[i]->HasComponent<Collider2DComponent>())
						continue;

					Collider2DComponent& collider = myMenuHandler.myObjectManager.myObjects[i]->GetComponent<Collider2DComponent>();
					collider.SetShouldRenderColliders(myShouldShowMenuColldiers);
				}
			}

			ImGui::MenuItem("Show Debug Data", NULL, &myShouldShowDebugData);

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
		newMenuName = "(Untitled)";
		ImGui::OpenPopup("Create New");
		myPopups.reset();
	}

	if (myPopups[(int)ePopup::SaveFileAs])
	{
		newMenuName = myMenuHandler.GetName() + " Copy";
		ImGui::OpenPopup("Save As");
		myPopups.reset();
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

			size_t n = newMenuName.find(".json");
			if (n == std::string::npos)
				newMenuName += ".json";

			std::string path = RELATIVE_IMPORT_DATA_PATH + MENU::MENU_PATH + newMenuName;
			nlohmann::json menu;
			std::ofstream dataFile(path);
			dataFile << menu;
			dataFile.close();

			myMenuHandler.Init(newMenuName, nullptr);
			FE::PostMaster::GetInstance()->SendMessage({ FE::eMessageType::NewMenuLoaded, newMenuName });
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Save As", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Enter Menu name: ");
		ImGui::InputText("##", &newMenuName, ImGuiInputTextFlags_AutoSelectAll);

		if (ImGui::Button("Save", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();

			myMenuHandler.SetName(newMenuName.substr(0, newMenuName.find_last_of('.')));

			size_t n = newMenuName.find(".json");
			if (n == std::string::npos)
				newMenuName += ".json";

			myMenuHandler.SaveToJson();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
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

		for (size_t i = 0; i < myAssets.textures.size(); i++)
		{
			if (myAssets.textureIdToName[i] == filename)
			{
				PrintI("Asset: " + filename + " already exists!");
				return;
			}
		}

		myAssets.textures.push_back(TextureFactory::CreateTexture(RELATIVE_SPRITE_ASSET_PATH + filename, false));
		myAssets.textureIdToName.push_back(filename);
		myAssets.textureNameToId[filename] = (UINT)myAssets.textures.size() - 1;
		break;
	}
	case FE::eMessageType::TtfDropped:
	{
		std::string filename = std::any_cast<std::string>(aMessage.myMessage);
	
		for (size_t i = 0; i < myAssets.fontFiles.size(); i++)
		{
			if (myAssets.fontFiles[i] == filename)
			{
				PrintI("Asset: " + filename + " already exists!");
				return;
			}
		}
		
		myAssets.fontFiles.push_back(filename);
		break;
	}
	case FE::eMessageType::PushEntityToInspector:
	{
		mySelectedObjectID = std::any_cast<ID>(aMessage.myMessage);
		break;
	}
	case FE::eMessageType::UpdateMenuEditorColliders:
	{
		GenerateEditorColliders();
		mySelectedObjectID = INVALID_ID;
		break;
	}
	case FE::eMessageType::NewMenuLoaded:
	{
		GenerateEditorColliders();
		mySelectedObjectID = INVALID_ID;
		break;
	}
	default:
		break;
	}
}
