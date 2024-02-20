#include "pch.h"
#include "MenuArrowButton.h"
#include <engine/Paths.h>
#include <engine/utility/Error.h>
#include <engine/graphics/sprite/Sprite.h>
#include <engine/graphics/GraphicsEngine.h>
#include <assets/AssetDatabase.h>
#include <engine/graphics/sprite/SpriteDrawer.h>
#include "../MenuHandler.h"
#include <assets/textureFactory.h>
#include <engine/text/text.h>

MenuArrowButton::MenuArrowButton(eButtonType aType, const std::string& aTitle)
	: MenuItem(aType)
{
	myTitleText = new Text(L"JuliusSansOne-Regular.ttf", FontSize_18);
	myVariousText = new Text(L"JuliusSansOne-Regular.ttf", FontSize_18);
	myTitleString = aTitle;

	//TODO TOVE: There is 100% a better way to do this, will fix in next Project!
	if (aType == eButtonType::Volume)
	{
		//int volume = static_cast<int>(std::floor(MainSingleton::GetAudioManager()->GetMasterVolume() * 100.f));
		//myVariousTextString = std::to_string(volume);
	}

	if (aType == eButtonType::Resolution)
	{
		Vector2i currentRes = { static_cast<int>(myRenderSize.x), static_cast<int>(myRenderSize.y) };
		myVariousTextString = std::to_string(currentRes.x) + "x" + std::to_string(currentRes.y);
	}
}

MenuArrowButton::~MenuArrowButton()
{
}

void MenuArrowButton::Init(std::vector<std::string> aTexture)
{
	auto texFac = AssetDatabase::GetTextureFactory();
	myTextures[static_cast<eButtonTextureType>(LEFT_ARROW_DEFAULT)].texture = texFac->CreateTexture(RELATIVE_SPRITE_ASSET_PATH + (std::string)"MenuUI/" + aTexture[LEFT_ARROW_DEFAULT]);
	myTextures[static_cast<eButtonTextureType>(LEFT_ARROW_HOVERED)].texture = texFac->CreateTexture(RELATIVE_SPRITE_ASSET_PATH + (std::string)"MenuUI/" + aTexture[LEFT_ARROW_HOVERED]);
	myTextures[static_cast<eButtonTextureType>(RIGHT_ARROW_DEFAULT)].texture = texFac->CreateTexture(RELATIVE_SPRITE_ASSET_PATH + (std::string)"MenuUI/" + aTexture[RIGHT_ARROW_DEFAULT]);
	myTextures[static_cast<eButtonTextureType>(RIGHT_ARROW_HOVERED)].texture = texFac->CreateTexture(RELATIVE_SPRITE_ASSET_PATH + (std::string)"MenuUI/" + aTexture[RIGHT_ARROW_HOVERED]);

	myLeftInstanceData.scale = myTextures[static_cast<eButtonTextureType>(LEFT_ARROW_DEFAULT)].texture->GetTextureSize();
	myLeftCollider.ReadDataFromSprite(myLeftInstanceData, myTextures[static_cast<eButtonTextureType>(LEFT_ARROW_DEFAULT)]);

	myRightInstanceData.scale = myTextures[static_cast<eButtonTextureType>(RIGHT_ARROW_DEFAULT)].texture->GetTextureSize();
	myRightCollider.ReadDataFromSprite(myRightInstanceData, myTextures[static_cast<eButtonTextureType>(RIGHT_ARROW_DEFAULT)]);

	myTitleText->SetText(myTitleString);

	if (myType == eButtonType::Volume)
	{
		myVariousText->SetText(myVariousTextString + "%");
	}

	if (myType == eButtonType::Resolution)
	{
		myVariousText->SetText(myVariousTextString);
	}

}

void MenuArrowButton::Update()
{
}

void MenuArrowButton::Render(SpriteDrawer& aSpriteDrawer)
{
	if (myLeftCollider.myIsHovered)
	{
		aSpriteDrawer.Draw(myTextures[static_cast<eButtonTextureType>(LEFT_ARROW_HOVERED)], myLeftInstanceData);
	}
	else
	{
		aSpriteDrawer.Draw(myTextures[static_cast<eButtonTextureType>(LEFT_ARROW_DEFAULT)], myLeftInstanceData);
	}

	if (myRightCollider.myIsHovered)
	{
		aSpriteDrawer.Draw(myTextures[static_cast<eButtonTextureType>(RIGHT_ARROW_HOVERED)], myRightInstanceData);
	}
	else
	{
		aSpriteDrawer.Draw(myTextures[static_cast<eButtonTextureType>(RIGHT_ARROW_DEFAULT)], myRightInstanceData);
	}

	myTitleText->Render();
	myVariousText->Render();
}

void MenuArrowButton::OnClick()
{
	static Vector2i resolutions[2] = { {1600,900}, {1920, 1080} };
	static int resolutionIndex = 0;
	const int maxIndex = 1;

	switch (myType)
	{
	case eButtonType::Resolution:
	{
		if (myLeftCollider.myIsHovered)
		{
			resolutionIndex--;
			if (resolutionIndex < 0)
			{
				resolutionIndex = maxIndex;
			}
		}
		else if (myRightCollider.myIsHovered)
		{
			resolutionIndex++;
			if (resolutionIndex > maxIndex)
			{
				resolutionIndex = 0;
			}
		}
		GraphicsEngine::GetInstance()->SetResolution(resolutions[resolutionIndex]);

		myVariousTextString = std::to_string(resolutions[resolutionIndex].x) + "x" + std::to_string(resolutions[resolutionIndex].y);
		break;
	}
	case eButtonType::Volume:
	{
		//auto audio = MainSingleton::GetAudioManager();
		//int currentVolume = static_cast<int>(audio->GetMasterVolume() * 100.f);
		//if (myLeftCollider.myIsHovered)
		//{
		//	currentVolume -= 10;
		//	if (currentVolume < 0)
		//	{
		//		currentVolume = 0;
		//	}
		//}
		//else if (myRightCollider.myIsHovered)
		//{
		//	currentVolume += 10;
		//	if (currentVolume > 100)
		//	{
		//		currentVolume = 100;
		//	}
		//}
		//audio->SetMasterVolume(currentVolume / 100.f);
		//myVariousTextString = std::to_string(currentVolume) + "%";
		break;
	}
	default:
		break;
	}

	myVariousText->SetText(myVariousTextString);
	myVariousText->CenterTextOverPosition(myTextPosition);

}

bool MenuArrowButton::IsHovered(const Vector2f& aMousePos)
{
	bool left = myLeftCollider.IsHovered(aMousePos);
	bool right = myRightCollider.IsHovered(aMousePos);

	return left || right;
}

void MenuArrowButton::SetPosition(const Vector2f& aPosition)
{
	myTextPosition = aPosition;
	myTitleText->SetPosition({ aPosition.x, aPosition.y });
	myTitleText->CenterTextOverPosition(aPosition);

	float textHeightOffset = 25.f;
	myVariousText->SetPosition({ aPosition.x, aPosition.y - textHeightOffset });
	myVariousText->CenterTextOverPosition(aPosition);

	myLeftInstanceData.position = { aPosition.x - DISTANCE_BETWEEN_BUTTONS, aPosition.y };
	myRightInstanceData.position = { aPosition.x + DISTANCE_BETWEEN_BUTTONS, aPosition.y };
	myLeftCollider.ReadDataFromSprite(myLeftInstanceData, myTextures[static_cast<eButtonTextureType>(LEFT_ARROW_DEFAULT)]);
	myRightCollider.ReadDataFromSprite(myRightInstanceData, myTextures[static_cast<eButtonTextureType>(RIGHT_ARROW_DEFAULT)]);
}

void MenuArrowButton::SetScale(const Vector2f& aScaleMultiplier)
{
	myLeftInstanceData.scaleMultiplier = aScaleMultiplier;
	myRightInstanceData.scaleMultiplier = aScaleMultiplier;
	myLeftCollider.ReadDataFromSprite(myLeftInstanceData, myTextures[static_cast<eButtonTextureType>(LEFT_ARROW_DEFAULT)]);
	myRightCollider.ReadDataFromSprite(myRightInstanceData, myTextures[static_cast<eButtonTextureType>(RIGHT_ARROW_DEFAULT)]);
}

void MenuArrowButton::SetTexturePath(eButtonTextureType aType, const std::string& aTexturePath)
{
	UNREFERENCED_PARAMETER(aType);
	UNREFERENCED_PARAMETER(aTexturePath);
}

void MenuArrowButton::ReloadTexture(eButtonTextureType aType)
{
	UNREFERENCED_PARAMETER(aType);
}
