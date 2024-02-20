#pragma once
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <nlohmann/json_fwd.hpp>
#include <engine/math/Vector.h>
#include "GuiCommon.h"
#include "MenuState.h"

#define CHAR_SIZE 128

class MenuButton;
class MenuItem;
class StateStack;
class Text;

enum class eMenuType
{
	PauseMenu,
	MainMenu,
	Count
};

class MenuHandler
{
	friend class MenuEditor;

public:
	MenuHandler(StateStack* aStateStack);
	~MenuHandler();

	void Init(eMenuType aType);
	void Update();
	void Render();

	void PushMenuState(eMenuState aState);
	void PopMenu();

private:
	void InitButtons();
	void InitMenuStates();
	void ArrangeButtonPositions();

	void ReadMenuData(const std::string& aFileName);
	void ReadSpriteData(GuiSprite& aSprite, const nlohmann::json& aJson);

	MenuState* GetCurrentMenuState() { return myMenuStack.top(); }
	std::vector<MenuItem*> GetCurrentButtons() { return myMenuStack.top()->myButtons; }

	std::string SAVE_FILE = "menu/menuData.json";

	StateStack* myStateStackPtr;

	eMenuType myType;
	std::string myMenuTypeName;
	std::stack<MenuState*> myMenuStack;

	std::map<eMenuState, MenuState> myMenuStates;
	std::map<eButtonType, MenuItem*> myButtons;
	std::map<eButtonType, std::vector<std::string>> myButtonTextures;

	Vector2f myButtonScale;
	Vector2f myScreenCenter;
	Vector2f myRenderSize;

	GuiSprite myBackground;

	// vvv DEBUG/EDITOR ONLY
	bool myShowMenuEditor;
	void WriteMenuData(const std::string& aFileName);
	void WriteSpriteData(const GuiSprite& aSprite, nlohmann::json& aJson);
	void SpriteTextureEdit(SpriteSharedData aSharedTexture, char aChar[CHAR_SIZE]);
	void ButtonTextureEdit(eButtonTextureType aTextureType, char aChar[CHAR_SIZE], int aButtonIndex);
	// ^^^ DEBUG/EDITOR ONLY
};