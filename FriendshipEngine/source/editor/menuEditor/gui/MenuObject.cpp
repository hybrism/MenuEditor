#include "MenuObject.h"

#include "components/MenuComponent.h"

#include "components/Collider2DComponent.h"
#include "components/SpriteComponent.h"
#include "components/TextComponent.h"

MENU::MenuObject::MenuObject(const unsigned int aID, const Vector2f& aPosition)
	: myID(aID)
	, myName("(Untitled)")
	, myPosition(aPosition)
{
}

void MENU::MenuObject::AddComponentOfType(ComponentType aType)
{
	switch (aType)
	{
	case MENU::ComponentType::Sprite:
		AddComponent<SpriteComponent>();
		break;
	case MENU::ComponentType::Collider2D:
		AddComponent<Collider2DComponent>();
		break;
	case MENU::ComponentType::Text:
		AddComponent<TextComponent>();
		break;
	default:
		break;
	}
}

void MENU::MenuObject::Init()
{
}

void MENU::MenuObject::Update()
{
	for (size_t typeIndex = 0; typeIndex < myComponents.size(); typeIndex++)
	{
		assert(!myComponents[typeIndex].empty() && "This should never happen");

		for (size_t componentIndex = 0; componentIndex < myComponents[typeIndex].size(); componentIndex++)
			myComponents[typeIndex][componentIndex]->Update();
	}
}

void MENU::MenuObject::Render()
{
	for (size_t typeIndex = 0; typeIndex < myComponents.size(); typeIndex++)
	{
		assert(!myComponents[typeIndex].empty() && "This should never happen");

		for (size_t componentIndex = 0; componentIndex < myComponents[typeIndex].size(); componentIndex++)
			myComponents[typeIndex][componentIndex]->Render();
	}
}

bool MENU::MenuObject::IsHovered()
{
	if (!HasComponent<Collider2DComponent>())
		return false;

	Collider2DComponent& collider = GetComponent<Collider2DComponent>();
	return collider.IsHovered();
}

void MENU::MenuObject::SetPosition(const Vector2f& aPosition)
{
	myPosition = aPosition;

	for (size_t typeIndex = 0; typeIndex < myComponents.size(); typeIndex++)
	{
		assert(!myComponents[typeIndex].empty() && "This should never happen");

		for (size_t componentIndex = 0; componentIndex < myComponents[typeIndex].size(); componentIndex++)
			myComponents[typeIndex][componentIndex]->UpdatePosition();
	}
}