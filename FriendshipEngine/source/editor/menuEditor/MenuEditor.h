#pragma once
#include <array>
#include <engine/math/Vector.h>
#include <shared/postMaster/Observer.h>
#include <assets/TextureFactory.h>
#include "windows/Window.h"

#include "gui/menuObject/ObjectManager.h"
#include "gui/MenuHandler.h"

class Texture;

namespace ME
{
	enum class ePopup
	{
		CreateNew,
		SaveFileAs,
		Count
	};

	const std::string RELATIVE_MENUEDITOR_ASSETS = "../../content/menuEditor/";
	const std::string SPRITES = "sprites/";
	const std::string MENU_PATH = "menus/";
	const std::string FONT_PATH = "fonts/";

	class MenuEditor : public FE::Observer
	{
		struct Assets
		{
			std::vector<Texture*> textures;
			std::vector<std::string> fontFiles;
			std::vector<std::string> saveFiles;
		} myAssets;

	public:
		MenuEditor();
		~MenuEditor();

		void Init();
		void Update(float dt);
		void Render();


	private: //IMGUI
		std::array<std::shared_ptr<ME::WindowBase>, (int)ME::ID::Count> myWindows;
		std::array<bool, (int)ePopup::Count> myPopups;

		bool myFirstFrameSetup;
		void Dockspace();
		void MenuBar();
		void Popups();
		void SaveFile();

	private:
		ObjectManager myObjectManager;
		TextureFactory myTextureFactory;
		MenuHandler myMenuHandler;

		Vector2f myRenderSize;
		Vector2f myRenderCenter;

		void RecieveMessage(const FE::Message& aMessage) override;
	};

}