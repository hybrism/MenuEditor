#include "pch.h"
#include "MenuHandler.h"

#include <engine/Paths.h>
#include <engine/utility/Error.h>
#include <engine/graphics/sprite/SpriteDrawer.h>
#include <assets/AssetDatabase.h>
#include <assets/textureFactory.h>
#include <engine/graphics/GraphicsEngine.h>

#include <engine/utility/InputManager.h>

#include "../utility/JsonUtility.h"
#include <nlohmann/json.hpp>

#include "menuItem/MenuButton.h"

MenuHandler::MenuHandler(StateStack* aStateStack) :
	myButtonScale({ 0.f, 0.f }),
	myStateStackPtr(aStateStack),
	myShowMenuEditor(true),
	myType(eMenuType::Count)
{}

MenuHandler::~MenuHandler()
{
	myStateStackPtr = nullptr;

	//for (int i = 0; i < static_cast<int>(eButtonType::Count); i++)
	//{
	//	delete myButtons[static_cast<eButtonType>(i)];
	//}
}

void MenuHandler::Init(eMenuType aType)
{
	myRenderSize.x = static_cast<float>(GraphicsEngine::GetInstance()->GetWindowDimensions().x);
	myRenderSize.y = static_cast<float>(GraphicsEngine::GetInstance()->GetWindowDimensions().y);
	myScreenCenter = myRenderSize / 2.f;
	myType = aType;

	ReadMenuData(SAVE_FILE);

	InitButtons();
	InitMenuStates();

	//TODO TOVE: This is not pretty! Will develop further
	switch (aType)
	{
	case eMenuType::PauseMenu:
		PushMenuState(eMenuState::PauseMenu);
		myBackground.myInstance.color = BACKGROUND_OPACITY;
		break;
	case eMenuType::MainMenu:
		PushMenuState(eMenuState::MainMenu);
		break;
	default:
		PrintW("[MenuHandler.cpp] Type is invalid");
		break;
	}

	auto texFac = AssetDatabase::GetTextureFactory();
	myBackground.mySharedData.texture = texFac->CreateTexture(RELATIVE_SPRITE_ASSET_PATH + (std::string)"MenuUI/" + myBackground.myTexturePath);
	myBackground.myInstance.scale = myBackground.GetTextureSize() * 10.f;
}

void MenuHandler::Update()
{
	auto input = InputManager::GetInstance();
	Vector2f mousePos = input->GetCurrentMousePositionVector2f();

	for (auto button : GetCurrentButtons())
	{
		button->Update();

		if (button->IsHovered(mousePos))
		{
			if (input->IsKeyPressed(VK_LBUTTON))
			{
				button->OnClick();
			}
		}
	}
}

void MenuHandler::Render()
{
	SpriteDrawer& spriteDrawer = GraphicsEngine::GetInstance()->GetSpriteDrawer();
	GraphicsEngine::GetInstance()->PrepareForSpriteRender();

	myBackground.Render(spriteDrawer);

	for (auto sprite : GetCurrentMenuState()->mySpriteTextures)
	{
		sprite->Render(spriteDrawer);
	}

	for (auto button : GetCurrentButtons())
	{
		button->Render(spriteDrawer);
	}

	GraphicsEngine::GetInstance()->ResetToDefault();
}

void MenuHandler::PushMenuState(eMenuState aType)
{
	myMenuStack.push(&myMenuStates[aType]);
	ArrangeButtonPositions();
}

void MenuHandler::PopMenu()
{
	myMenuStack.pop();
}

void MenuHandler::InitButtons()
{
	MenuItemFactory menuItemFactory(myStateStackPtr, this);

	for (int i = 0; i < static_cast<int>(eButtonType::Count); i++)
	{
		eButtonType type = static_cast<eButtonType>(i);
		myButtons[type] = menuItemFactory.CreateMenuItem(type);

		//vvv TODO TOVE: if we set textures beforehand buttons already know which texture it should have
		//Maybe Set type in constructor aswell and remove init completely? hmm.
		myButtons[type]->Init(myButtonTextures[type]);
	}
}

void MenuHandler::InitMenuStates()
{
	myMenuStates[eMenuState::MainMenu].Init(eMenuState::MainMenu, myButtons);
	myMenuStates[eMenuState::PauseMenu].Init(eMenuState::PauseMenu, myButtons);
	myMenuStates[eMenuState::Settings].Init(eMenuState::Settings, myButtons);
	myMenuStates[eMenuState::LevelSelect].Init(eMenuState::LevelSelect, myButtons);
	myMenuStates[eMenuState::Credits].Init(eMenuState::Credits, myButtons);
}

void MenuHandler::ArrangeButtonPositions()
{
	for (int i = 0; i < GetCurrentButtons().size(); i++)
	{
		GetCurrentButtons()[i]->SetPosition({ GetCurrentMenuState()->myTopButtonPosition.x, (GetCurrentMenuState()->myTopButtonPosition.y - (i * GetCurrentMenuState()->myButtonSpacing)) });
		GetCurrentButtons()[i]->SetScale(myButtonScale);
	}
}

void MenuHandler::ReadMenuData(const std::string& aFileName)
{
	switch (myType)
	{
	case eMenuType::PauseMenu:
		myMenuTypeName = "pauseMenu";
		break;
	case eMenuType::MainMenu:
		myMenuTypeName = "mainMenu";
		break;
	default:
		PrintE("[MenuHandler.cpp] Menu type is invalid!");
		break;
	}

	nlohmann::json jsonData = JsonUtility::OpenJson(aFileName);

	nlohmann::json menuData = jsonData[myMenuTypeName];
	for (int i = 0; i < menuData["menuData"].size(); i++)
	{
		eMenuState type = static_cast<eMenuState>(i);
		nlohmann::json m = menuData["menuData"][i];
		myMenuStates[type].myMenuStateName = m["stateType"].get<std::string>();
		myMenuStates[type].myButtonSpacing = m["spacing"].get<float>() * myRenderSize.y;
		myMenuStates[type].myTopButtonPosition.x = m["xAlignment"].get<float>() * myRenderSize.x;
		myMenuStates[type].myTopButtonPosition.y = m["topPos"].get<float>() * myRenderSize.y;

		for (int spriteIndex = 0; spriteIndex < myMenuStates[type].mySpriteTextures.size(); spriteIndex++)
		{
			delete myMenuStates[type].mySpriteTextures[spriteIndex];
		}
		myMenuStates[type].mySpriteTextures.clear();

		for (int j = 0; j < m["sprites"].size(); j++)
		{
			GuiSprite* sprite = new GuiSprite();
			ReadSpriteData(*sprite, m["sprites"][j]);
			std::string relative = RELATIVE_SPRITE_ASSET_PATH + (std::string)"MenuUI/";
			sprite->mySharedData.texture = AssetDatabase::GetTextureFactory()->CreateTexture(relative + sprite->myTexturePath);
			sprite->myInstance.scale = sprite->GetTextureSize();
			myMenuStates[type].mySpriteTextures.push_back(sprite);
		}

	}

	ReadSpriteData(myBackground, menuData["background"]);
	myBackground.myInstance.color = { 0.f, 0.f, 0.f, 1.f };

	nlohmann::json buttonData = jsonData["buttons"];
	myButtonScale.x = buttonData["scale"]["x"];
	myButtonScale.y = buttonData["scale"]["y"];
	for (int i = 0; i < buttonData["textures"].size(); i++)
	{
		myButtonTextures[static_cast<eButtonType>(i)] = buttonData["textures"][i];
	}

}

void MenuHandler::ReadSpriteData(GuiSprite& aSprite, const nlohmann::json& aJson)
{
	aSprite.myInstance.position.x = aJson["pos"]["x"].get<float>() * myRenderSize.x;
	aSprite.myInstance.position.y = aJson["pos"]["y"].get<float>() * myRenderSize.y;
	aSprite.myInstance.scaleMultiplier.x = aJson["scale"]["x"].get<float>();
	aSprite.myInstance.scaleMultiplier.y = aJson["scale"]["y"].get<float>();
	aSprite.myTexturePath = aJson["path"].get<std::string>();
}

#pragma region EDITOR FUNCTIONS

void MenuHandler::WriteMenuData(const std::string& aFileName)
{
	nlohmann::json jsonData = JsonUtility::OpenJson(aFileName);
	nlohmann::json& menuData = jsonData[myMenuTypeName];
	menuData.clear();

	for (int i = 0; i < static_cast<int>(eMenuState::Count); i++)
	{
		eMenuState type = static_cast<eMenuState>(i);
		menuData["menuData"][i]["stateType"] = myMenuStates[type].myMenuStateName;
		menuData["menuData"][i]["spacing"] = myMenuStates[type].myButtonSpacing / myRenderSize.y;
		menuData["menuData"][i]["xAlignment"] = myMenuStates[type].myTopButtonPosition.x / myRenderSize.x;
		menuData["menuData"][i]["topPos"] = myMenuStates[type].myTopButtonPosition.y / myRenderSize.y;

		for (int j = 0; j < myMenuStates[type].mySpriteTextures.size(); j++)
		{
			nlohmann::json sprite;
			WriteSpriteData(*myMenuStates[type].mySpriteTextures[j], sprite);
			menuData["menuData"][i]["sprites"].push_back(sprite);
		}
	}

	WriteSpriteData(myBackground, menuData["background"]);

	//SHARED For all buttons
	nlohmann::json& buttonData = jsonData["buttons"];
	buttonData["scale"]["x"] = myButtonScale.x;
	buttonData["scale"]["y"] = myButtonScale.y;

	for (int i = 0; i < static_cast<int>(eButtonType::Count); i++)
	{
		buttonData["textures"][i] = myButtonTextures[static_cast<eButtonType>(i)];
	}

	JsonUtility::OverwriteJson(aFileName, jsonData);
}

void MenuHandler::WriteSpriteData(const GuiSprite& aSprite, nlohmann::json& aJson)
{
	aJson["pos"]["x"] = aSprite.myInstance.position.x / myRenderSize.x;
	aJson["pos"]["y"] = aSprite.myInstance.position.y / myRenderSize.y;
	aJson["scale"]["x"] = aSprite.myInstance.scaleMultiplier.x;
	aJson["scale"]["y"] = aSprite.myInstance.scaleMultiplier.y;
	aJson["path"] = aSprite.myTexturePath;
}

void MenuHandler::SpriteTextureEdit(SpriteSharedData aSharedTexture, char aChar[CHAR_SIZE])
{
	aSharedTexture;
	aChar;

#ifdef _DEBUG
	//ImGui::PushID(&aChar);
	//ImGui::InputTextWithHint("", "Starts at Resources/Sprites/...", aChar, CHAR_SIZE);
	//ImGui::SameLine();
	//if (ImGui::Button("Reload"))
	//{
	//	auto texFac = AssetDatabase::GetTextureFactory();
	//	Texture* newTexture = texFac->CreateTexture(aChar);
	//	if (newTexture != nullptr)
	//	{
	//		delete aSharedTexture.texture;
	//		aSharedTexture.texture = newTexture;
	//	}
	//}
	//ImGui::PopID();
#endif
}

void MenuHandler::ButtonTextureEdit(eButtonTextureType aTextureType, char aChar[CHAR_SIZE], int aButtonIndex)
{
	aTextureType;
	aButtonIndex;
	aChar;

#ifdef _DEBUG
	//ImGui::PushID(static_cast<int>(aTextureType));
	//ImGui::InputTextWithHint("", "Starts at MenuUI/...", aChar, CHAR_SIZE);
	//ImGui::SameLine();
	//if (ImGui::Button("Reload"))
	//{
	//	eButtonType type = GetCurrentButtons()[aButtonIndex]->GetType();

	//	//vvv TODO: This step is obsolete if we set textures to buttons directly
	//	myButtonTextures[myButtons[type]->GetType()][static_cast<int>(aTextureType)] = aChar;
	//	//^^^

	//	myButtons[type]->SetTexturePath(aTextureType, aChar);
	//	myButtons[type]->ReloadTexture(aTextureType);
	//	myButtons[type]->SetScale(myButtonScale);
	//}
	//ImGui::PopID();
#endif
}

#pragma endregion