#include "pch.h"

#include "MenuObject.h"

#include "components/MenuComponent.h"
#include "components/Collider2DComponent.h"
#include "components/SpriteComponent.h"
#include "components/TextComponent.h"
#include "components/CommandComponent.h"
#include "components/InteractableComponent.h"


MENU::MenuObject::MenuObject(const unsigned int aID, const Vector2f& aPosition)
	: myID(aID)
	, myName("(Untitled)")
	, myPosition(aPosition)
	, myInitialPosition(aPosition)
	, myState(ObjectState::Default)
{}

MENU::MenuObject::~MenuObject()
{}

void MENU::MenuObject::RemoveComponent(const ID aID)
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		if (myComponents[i]->GetID() == aID)
		{
			myComponents.erase(myComponents.begin() + i);
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
	case MENU::ComponentType::Interactable:
		AddComponent<InteractableComponent>();
		break;
	default:
		break;
	}
}

void MENU::MenuObject::Update(const MenuUpdateContext& aContext)
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Update(aContext);
	}

	myState = ObjectState::Default;

	if (IsHovered())
	{
		myState = ObjectState::Hovered;

		if (IsPressed())
			myState = ObjectState::Pressed;
	}
}

void MENU::MenuObject::Render()
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Render();
	}
}

void MENU::MenuObject::OnResize(const Vector2i& aNewRenderSize)
{
	Vector2f scaleRatio = {
		(float)aNewRenderSize.x / (float)myTargetRenderSize.x,
		(float)aNewRenderSize.y / (float)myTargetRenderSize.y
	};

	Vector2f positionRatio = {
		myInitialPosition.x * scaleRatio.x,
		myInitialPosition.y * scaleRatio.y
	};

	//myPosition = { positionRatio.x * aNewRenderSize.x, positionRatio.y * aNewRenderSize.y };
	myPosition = positionRatio;

	for (size_t i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->OnResize(scaleRatio);
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

void MENU::MenuObject::SetPosition(const Vector2f& aPosition, bool aIsInitialPosition)
{
	myPosition = aPosition;
	if (aIsInitialPosition)
	{
		myInitialPosition = aPosition;
		myTargetRenderSize = GraphicsEngine::GetInstance()->DX().GetViewportDimensions();
	}

	for (size_t i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->UpdatePosition();
	}
}