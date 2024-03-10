#include "CommandComponent.h"
#include "../MenuObject.h"
#include <engine/utility/Error.h>

MENU::CommandComponent::CommandComponent(MenuObject& aParent, unsigned int aID)
	: MenuComponent(aParent, aID, ComponentType::Command)
{
	myCommandType = eCommandType::Count;
}

void MENU::CommandComponent::Update(const MenuUpdateContext& aContext)
{
	aContext;
}

void MENU::CommandComponent::SetCommandType(eCommandType aType)
{
	myCommandType = aType;
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
