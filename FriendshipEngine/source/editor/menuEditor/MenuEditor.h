#pragma once
#include "gui/MenuHandler.h"
#include "MenuCommon.h"
#include "windows/Window.h"
#include <array>
#include <assets/TextureFactory.h>
#include <engine/math/Vector.h>
#include <shared/postMaster/Observer.h>

class Game;
class Texture;

namespace MENU
{
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

	private: //IMGUI
		std::array<std::shared_ptr<MENU::WindowBase>, (int)MENU::ID::Count> myWindows;
		std::array<bool, (int)ePopup::Count> myPopups;

		bool myFirstFrameSetup;
		void Dockspace();
		void MenuBar();
		void Popups();

	private:
		ObjectManager myEditorObjectManager;
		MenuHandler myMenuHandler;
		TextureFactory myTextureFactory;
		Assets myAssets;

		size_t mySelectedEntityIndex;
		size_t myGizmoIndex;

		Vector2f myRenderSize;
		Vector2f myRenderCenter;

		void RecieveMessage(const FE::Message& aMessage) override;
	};

}