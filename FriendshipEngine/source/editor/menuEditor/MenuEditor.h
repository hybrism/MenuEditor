#pragma once
#include "gui/MenuHandler.h"
#include "MenuEditorCommon.h"
#include "windows/Window.h"
#include <array>
#include <map>
#include <engine/math/Vector.h>
#include <shared/postMaster/Observer.h>

class Game;
class Texture;

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

		unsigned int mySelectedObjectID;
		unsigned int myGizmoID;
		unsigned int myUpGizmoID;
		unsigned int myRightGizmoID;

		Vector2f myGizmoPosition;
		Vector2f myRenderSize;
		Vector2f myRenderCenter;

		//IMGUI
		std::array<std::shared_ptr<MENU::WindowBase>, (int)MENU::ID::Count> myWindows;
		std::array<bool, (int)ePopup::Count> myPopups;

		bool myFirstFrameSetup;
		void Dockspace();
		void MenuBar();
		void Popups();
		//!IMGUI
	
		//POSTMASTER
		void RecieveMessage(const FE::Message& aMessage) override;
		//!POSTMASTER
	};

}