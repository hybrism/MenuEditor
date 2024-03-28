#include "pch.h"

#include <variant>

#include "CommandComponent.h"
#include "CommandFunctions.h"

#include <engine/utility/Error.h>

#include "../MenuObject.h"


MENU::CommandComponent::CommandComponent(MenuObject& aParent, unsigned int aID)
	: MenuComponent(aParent, aID, ComponentType::Command)
{
	myCommandType = eCommandType::Count;
}

void MENU::CommandComponent::Execute(CommandData aData, const MenuUpdateContext& aContext)
{
	if (!myCommand)
	{
		PrintW("No command set!");
		return;
	}

	myCommand(aData, aContext);
}

void MENU::CommandComponent::SetCommandType(eCommandType aType)
{
	myCommandType = aType;

	switch (myCommandType)
	{
	case MENU::eCommandType::PopMenu:
		myCommand = PopMenuCommand;
		break;
	case MENU::eCommandType::PushMenu:
		myCommand = PushMenuCommand;
		break;
	case MENU::eCommandType::LoadLevel:
		myCommand = LoadSceneCommand;
		break;
	case MENU::eCommandType::ResumeGame:
		myCommand = ResumeGameCommand;
		break;
	case MENU::eCommandType::BackToMainMenu:
		myCommand = ReturnToMainMenuCommand;
		break;
	case MENU::eCommandType::QuitGame:
		myCommand = QuitGameCommand;
		break;
	default:
		break;
	}
}

void MENU::CommandComponent::SetCommandData(CommandData aData)
{
	myCommandData = aData;
}

MENU::eCommandType MENU::CommandComponent::GetCommandType() const
{
	return myCommandType;
}

MENU::CommandData MENU::CommandComponent::GetCommandData()
{
	return myCommandData;
}

