#pragma once
#include "MenuComponent.h"

namespace MENU
{
	class MenuObject;

	enum class eCommandType
	{
		PopMenu,
		PushSettingsMenu,
		PushLevelSelectMenu,
		LoadLevel3,
		LoadFeatureGym,
		QuitGame,
		Count
	};

	static const char* CommandNames[] =
	{
		"Pop Menu",
		"Push Settings Menu",
		"Push Level Select Menu",
		"Load Level 3",
		"Load Feature Gym",
		"Quit Game",
		"None"
	};
	
	class CommandComponent : public MenuComponent
	{
	public:
		CommandComponent(MenuObject& aParent, unsigned int aID);

		void Update(const MenuUpdateContext& aContext) override;

		void SetCommand(eCommandType aType);
		eCommandType GetCommand() const;

	private:
		eCommandType myCommandType;
	};
}