#pragma once
#include <map>
#include "MenuItem.h"

class Text;

class MenuArrowButton : public MenuItem
{
public:
	MenuArrowButton(eButtonType aType, const std::string& aName);
	~MenuArrowButton();

	void Init(std::vector<std::string> aTexture) override;
	void Update() override;

	void Render(SpriteDrawer& aSpriteDrawer) override;
	void OnClick() override;
	bool IsHovered(const Vector2f& aMousePos) override;
	void SetPosition(const Vector2f& aPosition) override;
	void SetScale(const Vector2f& aScaleMultiplier) override;
	void SetTexturePath(eButtonTextureType aType, const std::string& aTexturePath) override;
	void ReloadTexture(eButtonTextureType aType) override;

private:
	const int LEFT_ARROW_DEFAULT = 0;
	const int LEFT_ARROW_HOVERED = 1;
	const int RIGHT_ARROW_DEFAULT = 2;
	const int RIGHT_ARROW_HOVERED = 3;

	const float DISTANCE_BETWEEN_BUTTONS = 150.f;

	Vector2f myTextPosition;
	Text* myTitleText;
	Text* myVariousText;
	std::string myTitleString;
	std::string myVariousTextString;

	//TODO TOVE: Slå ihop till std::vector
	//LEFT SPRITE
	GuiCollider myLeftCollider;
	SpriteInstanceData myLeftInstanceData;

	//RIGHT SPRITE
	GuiCollider myRightCollider;
	SpriteInstanceData myRightInstanceData;

	//Kan delas
	std::map<eButtonTextureType, SpriteSharedData> myTextures;

};