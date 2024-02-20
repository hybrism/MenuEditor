#include "pch.h"
//#include "MenuButton.h"
//
//#include <engine/Paths.h>
//#include <engine/utility/Error.h>
//#include <engine/graphics/sprite/Sprite.h>
//#include <engine/graphics/GraphicsEngine.h>
//#include <assets/AssetDatabase.h>
//#include <engine/graphics/sprite/SpriteDrawer.h>
//#include "../../stateStack/StateStack.h"
//#include <engine/text/text.h>
//#include "../MenuHandler.h"
//#include <assets/textureFactory.h>
//
//MenuButton::MenuButton(eButtonType aType, StateStack* aStateStack, MenuHandler* aMenuHandler)
//	: MenuItem(aType)
//{
//	myStateStackPtr = aStateStack;
//	myMenuHandlerPtr = aMenuHandler;
//	myText = new Text(L"JuliusSansOne-Regular.ttf", FontSize_30);
//	myTypeName = "";
//}
//
//MenuButton::~MenuButton()
//{
//	delete myText;
//	myStateStackPtr = nullptr;
//	myMenuHandlerPtr = nullptr;
//}
//
//void MenuButton::Init(std::vector<std::string> aTexture)
//{
//	//myTexturePaths[eButtonTextureType::Default] = aTexture[static_cast<int>(eButtonTextureType::Default)];
//	//myTexturePaths[eButtonTextureType::Hovered] = aTexture[static_cast<int>(eButtonTextureType::Hovered)];
//
//	// vvv WILL BE REMOVED WHEN WE GET BUTTONS FROM SG
//#pragma region TEXT
//	switch (myType)
//	{
//	case eButtonType::StartGame:
//		myTypeName = "Start Game";
//		break;
//	case eButtonType::LevelSelect:
//		myTypeName = "Level Select";
//		break;
//	case eButtonType::Settings:
//		myTypeName = "Settings";
//		break;
//	case eButtonType::Credits:
//		myTypeName = "Credits";
//		break;
//	case eButtonType::Back:
//		myTypeName = "Back";
//		break;
//	case eButtonType::Quit:
//		myTypeName = "Quit";
//		break;
//	case eButtonType::Resume:
//		myTypeName = "Resume";
//		break;
//	case eButtonType::Resolution:
//		myTypeName = "Resolution [WIP]";
//		break;
//	case eButtonType::Volume:
//		myTypeName = "Volume [WIP]";
//		break;
//	case eButtonType::BackToMain:
//		myTypeName = "Back to Main Menu";
//		break;
//	case eButtonType::VillageLevel:
//		myTypeName = "Village";
//		break;
//	case eButtonType::SwampLevel:
//		myTypeName = "Swamp";
//		break;
//	case eButtonType::DeepSwampLevel:
//		myTypeName = "DeepSwamp";
//		break;
//	case eButtonType::CaveLevel:
//		myTypeName = "Cave";
//		break;
//	case eButtonType::BossLevel:
//		myTypeName = "Boss";
//		break;
//	case eButtonType::Count:
//		PrintW("Button does not have a Type");
//		break;
//	default:
//		break;
//	}
//
//	myText->SetText(myTypeName);
//	myText->SetColor({ 1.f, 0.f, 1.f, 1.f });
//	myText->CenterTextOverPosition(myText->GetPosition());
//#pragma endregion
//	// ^^^ WILL BE REMOVED WHEN WE GET BUTTONS FROM SG
//
//	auto texFac = AssetDatabase::GetTextureFactory();
//	myTextures[eButtonTextureType::Default].texture = texFac->CreateTexture(RELATIVE_SPRITE_ASSET_PATH + (std::string)"MenuUI/" + aTexture[static_cast<int>(eButtonTextureType::Default)]);
//	myTextures[eButtonTextureType::Hovered].texture = texFac->CreateTexture(RELATIVE_SPRITE_ASSET_PATH + (std::string)"MenuUI/" + aTexture[static_cast<int>(eButtonTextureType::Hovered)]);
//	myInstanceData.scale = myTextures[eButtonTextureType::Default].texture->GetTextureSize();
//	
//	myCollider.ReadDataFromSprite(myInstanceData, myTextures[eButtonTextureType::Default]);
//
//}
//
//void MenuButton::Update()
//{}
//
//void MenuButton::Render(SpriteDrawer& aSpriteDrawer)
//{
//	if (myCollider.myIsHovered)
//	{
//		aSpriteDrawer.Draw(myTextures[eButtonTextureType::Hovered], myInstanceData);
//	}
//	else
//	{
//		aSpriteDrawer.Draw(myTextures[eButtonTextureType::Default], myInstanceData);
//	}
//
//	// vvv WILL BE REMOVED WHEN WE GET BUTTONS FROM SG
//	//myText->Render();
//	// ^^^ WILL BE REMOVED WHEN WE GET BUTTONS FROM SG
//}
//
//void MenuButton::OnClick()
//{
//	switch (myType)
//	{
//	case eButtonType::StartGame:
//		break;
//	case eButtonType::LevelSelect:
//		myMenuHandlerPtr->PushMenuState(eMenuState::LevelSelect);
//		break;
//	case eButtonType::Settings:
//		myMenuHandlerPtr->PushMenuState(eMenuState::Settings);
//		break;
//	case eButtonType::Credits:
//		myMenuHandlerPtr->PushMenuState(eMenuState::Credits);
//		break;
//	case eButtonType::Back:
//		myMenuHandlerPtr->PopMenu();
//		break;
//	case eButtonType::Quit:
//		//TODO TOVE: Quit game? This works when only used in mainmenu
//		myStateStackPtr->PopState();
//		break;
//	case eButtonType::Resume:
//		myStateStackPtr->PopState();
//		break;
//	case eButtonType::BackToMain:
//		while (myStateStackPtr->GetCurrentStateType() != eStateType::MainMenu)
//		{
//			myStateStackPtr->PopState();
//		}
//		break;
//	case eButtonType::Count:
//		PrintW("Button does not have a Type");
//		break;
//	default:
//		break;
//	}
//}
//
//void MenuButton::SetPosition(const Vector2f& aPosition)
//{
//	// vvv WILL BE REMOVED WHEN WE GET BUTTONS FROM SG
//	myText->SetPosition(aPosition);
//	myText->CenterTextOverPosition(myText->GetPosition());
//	// ^^^ WILL BE REMOVED WHEN WE GET BUTTONS FROM SG
//
//	myInstanceData.position = aPosition;
//	myCollider.ReadDataFromSprite(myInstanceData, myTextures[eButtonTextureType::Default]);
//}
//
//void MenuButton::SetScale(const Vector2f& aScaleMultiplier)
//{
//	myInstanceData.scaleMultiplier = aScaleMultiplier;
//	myCollider.ReadDataFromSprite(myInstanceData, myTextures[eButtonTextureType::Default]);
//}
//
//void MenuButton::SetTexturePath(eButtonTextureType aType, const std::string& aTexturePath)
//{
//	myTexturePaths[aType] = aTexturePath;
//}
//
//void MenuButton::ReloadTexture(eButtonTextureType aType)
//{
//	auto texFac = AssetDatabase::GetTextureFactory();
//	Texture* newTexture = texFac->CreateTexture(RELATIVE_SPRITE_ASSET_PATH + (std::string)"MenuUI/" + myTexturePaths[aType]);
//	if (newTexture == nullptr)
//	{
//		return;
//	}
//
//	delete myTextures[aType].texture;
//	myTextures[aType].texture = newTexture;
//	myInstanceData.scale = newTexture->GetTextureSize();
//}
//
//bool MenuButton::IsHovered(const Vector2f& aMousePos)
//{
//	return myCollider.IsHovered(aMousePos);
//}
