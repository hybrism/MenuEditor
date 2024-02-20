#pragma once
#include <vector>
#include "../GuiCommon.h"

class StateStack;
class MenuHandler;

class MenuItem
{
public:
	MenuItem(eButtonType aType);
	~MenuItem() = default;

	virtual void Init(std::vector<std::string> aTexture) = 0;
	virtual void Update() = 0;
	virtual void Render(SpriteDrawer& aSpriteDrawer) = 0;

	virtual void OnClick() = 0;
	virtual bool IsHovered(const Vector2f& aMousePos) = 0;

	virtual void SetPosition(const Vector2f& aPosition) = 0;
	virtual void SetScale(const Vector2f& aScaleMultiplier) = 0;
	virtual void SetTexturePath(eButtonTextureType aType, const std::string& aTexturePath) = 0;
	virtual void ReloadTexture(eButtonTextureType aType) = 0;

	virtual const Vector2f& GetPosition() { return myPosition; }
	virtual const Vector2f& GetScaleMultiplier() { return myScaleMultiplier; }
	virtual const eButtonType& GetType() { return myType; }
	virtual const std::string& GetTypeName() { return myTypeName; }

protected:
	Vector2f myRenderSize;
	Vector2f myScreenCenter;
	eButtonType myType;

	Vector2f myPosition;
	Vector2f myScaleMultiplier;
	std::string myTypeName;

};

class MenuItemFactory
{
public:
	MenuItemFactory(StateStack* aStateStack, MenuHandler* aMenuHandler);
	MenuItem* CreateMenuItem(const eButtonType aButtonType);

private:
	StateStack* myStateStackPtr;
	MenuHandler* myMenuHandlerPtr;
};