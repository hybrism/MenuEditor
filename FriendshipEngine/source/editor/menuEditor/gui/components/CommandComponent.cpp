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

void MENU::CommandComponent::SetCommand(eCommandType aType)
{
	myCommandType = aType;
}

MENU::eCommandType MENU::CommandComponent::GetCommand() const
{
	return myCommandType;
}
