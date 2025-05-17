#pragma once
#include <variant>
#include <string>
#include <functional>

#include "../IDManager.h"
#include "MenuComponent.h"

namespace MENU
{
	class MenuObject;

	enum class eCommandType
	{
		PopMenu,
		PushMenu,
		LoadLevel,
		ResumeGame,
		BackToMainMenu,
		QuitGame,
		Headbob,
		Fullscreen,
		Resolution,
		SfxVolume,
		MusicVolume,

		Count
	};

	static const char* CommandNames[] =
	{
		"Pop Menu",
		"Push Menu",
		"Load Level",
		"Resume Game",
		"Back to Main Menu",
		"Quit Game",
		"Headbob",
		"Fullscreen",
		"Resolution",
		"Sfx Volume",
		"Music Volume",

		"(None)"
	};

	struct CommandData
	{
		std::variant<std::monostate, std::string, ID, bool*, float, int, Vector2i*> data;
	};

	class CommandComponent : public MenuComponent
	{
	public:
		CommandComponent(MenuObject& aParent, unsigned int aID);

		void Execute(CommandData aData, const MenuUpdateContext& aContext);

		void SetCommandType(eCommandType aType);
		void SetCommandData(CommandData aData);

		eCommandType GetCommandType() const;
		CommandData GetCommandData();

	private:
		eCommandType myCommandType;
		CommandData myCommandData;

		std::function<void(CommandData, const MenuUpdateContext&)> myCommand;
	};
}