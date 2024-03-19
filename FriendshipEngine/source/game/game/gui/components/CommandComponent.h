#pragma once
#include <variant>
#include <string>

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

		"(None)"
	};
	
	struct CommandData
	{
		std::variant<std::monostate, std::string, ID> data;
	};

	class CommandComponent : public MenuComponent
	{
	public:
		CommandComponent(MenuObject& aParent, unsigned int aID);

		void Update(const MenuUpdateContext& aContext) override;

		void SetCommandType(eCommandType aType);
		void SetCommandData(CommandData aData);

		eCommandType GetCommandType() const;
		CommandData GetCommandData();

	private:
		eCommandType myCommandType;
		CommandData myCommandData;
	};
}