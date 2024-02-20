#pragma once
#include "GuiCommon.h"

class MenuButton;
class MenuItem;
class GuiSprite;

enum class eMenuState
{
	MainMenu,
	PauseMenu,
	Settings,
	LevelSelect,
	Credits,
	Count
};

class MenuState
{
public:
	MenuState();

	void Init(eMenuState aStateType, std::map<eButtonType, MenuItem*> aMenuItems);

	//private:
	eMenuState myStateType;

	std::vector<MenuItem*> myButtons;
	std::vector<GuiSprite*> mySpriteTextures;

	Vector2f myTopButtonPosition;
	float myButtonSpacing;


	//FOR DEBUG
	std::string myMenuStateName;
};