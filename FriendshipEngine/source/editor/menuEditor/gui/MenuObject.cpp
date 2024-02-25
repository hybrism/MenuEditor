#include "MenuObject.h"
#include "components/MenuComponent.h"
#include "components/Collider2DComponent.h"
#include "components/SpriteComponent.h"
#include "components/TextComponent.h"

MENU::MenuObject::MenuObject(const unsigned int aID, const Vector2f& aPosition)
	: myID(aID)
	, myName("untitled")
	, myPosition(aPosition)
{
}

void MENU::MenuObject::AddComponentOfType(eComponentType aType)
{
	switch (aType)
	{
	case MENU::eComponentType::Sprite:
	{
		std::shared_ptr<SpriteComponent> sprite = std::make_shared<SpriteComponent>(*this);
		myComponents.emplace_back(sprite);
		break;
	}
	case MENU::eComponentType::Collider2D:
	{
		std::shared_ptr<Collider2DComponent> collider = std::make_shared<Collider2DComponent>(*this);
		myComponents.emplace_back(collider);
		break;
	}
	case MENU::eComponentType::Text:
	{
		std::shared_ptr<TextComponent> text = std::make_shared<TextComponent>(*this);
		myComponents.emplace_back(text);
		break;
	}
	case MENU::eComponentType::Count:
	default:
		break;
	}
}

void MENU::MenuObject::Init()
{
}

void MENU::MenuObject::Update()
{
	for (int i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Update();
	}
}

void MENU::MenuObject::Render()
{
	for (int i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Render();
	}
}
