#pragma once
#include <string>
#include <vector>
#include <map>
#include <engine/math/Vector.h>
#include "MenuItem.h"
#include "../GuiCommon.h"

class Text;
class StateStack;
class SpriteDrawer;
class MenuHandler;

#define DEFAULT_TEXTURE_SLOT 0
#define HOVERED_TEXTURE_SLOT 1

class MenuButton : public MenuItem
{
public:
	MenuButton(eButtonType aType, StateStack* aStateStack, MenuHandler* aMenuHandler);
	~MenuButton();

	void Init(std::vector<std::string> aTexture) override;
	void Update() override;
	void Render(SpriteDrawer& aSpriteDrawer) override;

	void OnClick();

	bool IsHovered(const Vector2f& aMousePos) override;

	void SetPosition(const Vector2f& aPosition) override;
	void SetScale(const Vector2f& aScaleMultiplier) override;
	void SetTexturePath(eButtonTextureType aType, const std::string& aTexturePath) override;
	void ReloadTexture(eButtonTextureType aType) override;

	const Vector2f& GetPosition() override { return myInstanceData.position; }
	const Vector2f& GetScaleMultiplier() override {	return myInstanceData.scaleMultiplier; }
	const eButtonType& GetType() {	return myType; }
	const std::string& GetTypeName() {	return myTypeName; }

private:
	StateStack* myStateStackPtr;
	MenuHandler* myMenuHandlerPtr;

	GuiCollider myCollider;

	//SPRITE
	SpriteInstanceData myInstanceData;
	std::map<eButtonTextureType, SpriteSharedData> myTextures;
	
	//TEXTUREPATHS
	std::map<eButtonTextureType, std::string> myTexturePaths;
	
	// vvv WILL BE REMOVED WHEN WE GET BUTTONS FROM SG
	Text* myText;
	// ^^^ WILL BE REMOVED WHEN WE GET BUTTONS FROM SG
};
