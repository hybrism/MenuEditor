#include "pch.h"

#include "MenuItem.h"
#include <engine/utility/Error.h>
#include <engine/graphics/GraphicsEngine.h>
#include "MenuButton.h"
#include "MenuArrowButton.h"

MenuItem::MenuItem(eButtonType aType)
{
	myRenderSize.x = static_cast<float>(GraphicsEngine::GetInstance()->GetWindowDimensions().x);
	myRenderSize.y = static_cast<float>(GraphicsEngine::GetInstance()->GetWindowDimensions().y);
	myScreenCenter = myRenderSize / 2.f;
	myType = aType;
}

MenuItemFactory::MenuItemFactory(StateStack* aStateStack, MenuHandler* aMenuHandler)
{
	myStateStackPtr = aStateStack;
	myMenuHandlerPtr = aMenuHandler;
}

MenuItem* MenuItemFactory::CreateMenuItem(const eButtonType aButtonType)
{
	switch (aButtonType)
	{
	case eButtonType::StartGame:
		return new MenuButton(aButtonType, myStateStackPtr, myMenuHandlerPtr);
		break;
	case eButtonType::LevelSelect:
		return new MenuButton(aButtonType, myStateStackPtr, myMenuHandlerPtr);
		break;
	case eButtonType::Settings:
		return new MenuButton(aButtonType, myStateStackPtr, myMenuHandlerPtr);
		break;
	case eButtonType::Credits:
		return new MenuButton(aButtonType, myStateStackPtr, myMenuHandlerPtr);
		break;
	case eButtonType::Back:
		return new MenuButton(aButtonType, myStateStackPtr, myMenuHandlerPtr);
		break;
	case eButtonType::Quit:
		return new MenuButton(aButtonType, myStateStackPtr, myMenuHandlerPtr);
		break;
	case eButtonType::Resume:
		return new MenuButton(aButtonType, myStateStackPtr, myMenuHandlerPtr);
		break;
	case eButtonType::Resolution:
		return new MenuArrowButton(aButtonType, "Resolution");
		break;
	case eButtonType::Volume:
		return new MenuArrowButton(aButtonType, "Volume");
		break;
	case eButtonType::BackToMain:
		return new MenuButton(aButtonType, myStateStackPtr, myMenuHandlerPtr);
		break;
	case eButtonType::VillageLevel:
		return new MenuButton(aButtonType, myStateStackPtr, myMenuHandlerPtr);
		break;
	case eButtonType::SwampLevel:
		return new MenuButton(aButtonType, myStateStackPtr, myMenuHandlerPtr);
		break;
	case eButtonType::DeepSwampLevel:
		return new MenuButton(aButtonType, myStateStackPtr, myMenuHandlerPtr);
		break;
	case eButtonType::CaveLevel:
		return new MenuButton(aButtonType, myStateStackPtr, myMenuHandlerPtr);
		break;
	case eButtonType::BossLevel:
		return new MenuButton(aButtonType, myStateStackPtr, myMenuHandlerPtr);
		break;
	default:
		PrintE("[MenuItem.cpp] - Invalid ButtonType!");
		break;
	}

	return nullptr;
}
