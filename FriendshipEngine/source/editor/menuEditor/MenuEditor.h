#pragma once
#include <array>
#include <shared/postMaster/Observer.h>
#include <game/gui/MenuHandler.h>

#include "MenuEditorCommon.h"
#include "windows/Window.h"

class Game;

namespace MENU 
{
	const std::string MENU_PATH = "menus/";

	enum class ePopup
	{
		CreateNew,
		SaveFileAs,
		Count
	};

	class MenuEditor : public FE::Observer
	{
	public:
		MenuEditor();
		~MenuEditor();

		void Init();
		void Update(float dt);
		void Render();

	private:
		void GizmoUpdate();
		void GenerateEditorColliders();

		std::map<unsigned int, unsigned int> myEditorIDToMenuIDMap;
		std::map<unsigned int, unsigned int> myMenuIDToEditorIDMap;
		ObjectManager myEditorObjectManager;
		MenuHandler myMenuHandler;
		Assets myAssets;

		ID mySelectedObjectID;
		ID myEditorIDStartIndex;
		ID myGizmoID;

		Vector2f myGizmoPosition;
		Vector2f myRenderSize;
		Vector2f myRenderCenter;

		//IMGUI
		std::array<std::shared_ptr<MENU::WindowBase>, (int)MENU::WindowID::Count> myWindows;
		std::bitset<(int)ePopup::Count> myPopups;
		
		ImVec2 myContentRegionMin;

		bool myShouldShowDebugData;
		bool myShouldShowEditorColliders;
		bool myShouldShowMenuColldiers;
		bool myFirstFrameSetup;

		void Dockspace();
		void MenuBar();
		void Popups();
		//!IMGUI
	
		void RecieveMessage(const FE::Message& aMessage) override;
	};

}