#include "pch.h"
#include "MenuState.h"
#include <engine/utility/Error.h>
#include "menuItem/MenuItem.h"

MenuState::MenuState()
{
	myTopButtonPosition = { 0.f, 0.f };
	myButtonSpacing = 0.f;
}

void MenuState::Init(eMenuState aStateType, std::map<eButtonType, MenuItem*> aMenuItems)
{
	myStateType = aStateType;

	switch (myStateType)
	{
	case eMenuState::MainMenu:
		myMenuStateName = "mainMenu";
		myButtons.push_back(aMenuItems[eButtonType::StartGame]);
		myButtons.push_back(aMenuItems[eButtonType::LevelSelect]);
		myButtons.push_back(aMenuItems[eButtonType::Settings]);
		myButtons.push_back(aMenuItems[eButtonType::Credits]);
		myButtons.push_back(aMenuItems[eButtonType::Quit]);
		break;
	case eMenuState::PauseMenu:
		myMenuStateName = "pauseMenu";
		myButtons.push_back(aMenuItems[eButtonType::Resume]);
		myButtons.push_back(aMenuItems[eButtonType::Settings]);
		myButtons.push_back(aMenuItems[eButtonType::BackToMain]);
		break;
	case eMenuState::Settings:
		myMenuStateName = "settings";
		myButtons.push_back(aMenuItems[eButtonType::Resolution]);
		myButtons.push_back(aMenuItems[eButtonType::Volume]);
		myButtons.push_back(aMenuItems[eButtonType::Back]);
		break;
	case eMenuState::LevelSelect:
		myMenuStateName = "levelSelect";
		myButtons.push_back(aMenuItems[eButtonType::VillageLevel]);
		myButtons.push_back(aMenuItems[eButtonType::SwampLevel]);
		myButtons.push_back(aMenuItems[eButtonType::DeepSwampLevel]);
		myButtons.push_back(aMenuItems[eButtonType::CaveLevel]);
		myButtons.push_back(aMenuItems[eButtonType::BossLevel]);
		myButtons.push_back(aMenuItems[eButtonType::Back]);
		break;
	case eMenuState::Credits:
		myMenuStateName = "credits";
		myButtons.push_back(aMenuItems[eButtonType::Back]);
		break;
	default:
		PrintE("[MenuState.cpp] - Invalid MenuState-type!");
		break;
	}
}
