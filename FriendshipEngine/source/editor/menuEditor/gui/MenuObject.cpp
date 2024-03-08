#include "MenuObject.h"

#include "components/MenuComponent.h"
#include "components/Collider2DComponent.h"
#include "components/SpriteComponent.h"
#include "components/TextComponent.h"
#include "components/CommandComponent.h"

MENU::MenuObject::MenuObject(const unsigned int aID, const Vector2f& aPosition)
	: myID(aID)
	, myName("(Untitled)")
	, myPosition(aPosition)
{
}

MENU::MenuObject::~MenuObject()
{
	IDManager* idManager = IDManager::GetInstance();

	for (size_t i = 0; i < myComponents.size(); i++)
	{
		idManager->FreeID(myComponents[i]->GetID());
	}

	idManager->FreeID(myID);
}

void MENU::MenuObject::RemoveComponent(const ID aID)
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		if (myComponents[i]->GetID() == aID)
		{
			myComponents.erase(myComponents.begin() + i);
			IDManager::GetInstance()->FreeID(aID);
			return;
		}
	}
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
	case MENU::ComponentType::Command:
		AddComponent<CommandComponent>();
		break;
	default:
		break;
	}
}

void MENU::MenuObject::Init()
{
}

void MENU::MenuObject::Update(const MenuUpdateContext& aContext)
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Update(aContext);
	}
}

void MENU::MenuObject::Render()
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Render();
	}
}

bool MENU::MenuObject::IsHovered()
{
	if (!HasComponent<Collider2DComponent>())
		return false;

	return GetComponent<Collider2DComponent>().IsHovered();
}

bool MENU::MenuObject::IsPressed()
{
	if (!HasComponent<Collider2DComponent>())
		return false;

	return GetComponent<Collider2DComponent>().IsPressed();
}

void MENU::MenuObject::SetPosition(const Vector2f& aPosition)
{
	myPosition = aPosition;

	for (size_t i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->UpdatePosition();
	}
}