#pragma once
#include "MenuComponent.h"

class MenuObject;

class Collider2DComponent : public MenuComponent
{
public:
	Collider2DComponent(MenuObject& aParent)
		: MenuComponent(aParent)
	{}

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

private:


};
