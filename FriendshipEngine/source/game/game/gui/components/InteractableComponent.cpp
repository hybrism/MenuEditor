#include "pch.h"

#include "InteractableComponent.h"

#include <engine/math/helper.h>

#include "../MenuObject.h"

#include "SpriteComponent.h"
#include "TextComponent.h"

MENU::InteractableComponent::InteractableComponent(MenuObject& aParent, unsigned int aID)
	: MenuComponent(aParent, aID, ComponentType::Sprite)
{}

void MENU::InteractableComponent::Update(const MenuUpdateContext& aContext)
{
	for (size_t i = 0; i < myInteractions.size(); i++)
	{
		if (myParent.GetState() == ObjectState::Hovered)
			myInteractions[i]->OnHovered(aContext);

		if (myParent.GetState() == ObjectState::Pressed)
			myInteractions[i]->OnPressed(aContext);
	}
}

void MENU::InteractableComponent::AddInteraction(std::shared_ptr<MenuComponent> aComponent, InteractionType aType)
{
	switch (aType)
	{
	case MENU::InteractionType::Drag:
		myInteractions.push_back(std::make_shared<DragInteraction>(aComponent));
		break;
	case MENU::InteractionType::Clip:
		myInteractions.push_back(std::make_shared<ClipInteraction>(aComponent));
		break;
	case MENU::InteractionType::Hide:
		myInteractions.push_back(std::make_shared<HideInteraction>(aComponent));
		break;
	default:
		break;
	}

}

void MENU::InteractableComponent::RemoveInteraction()
{
}

// ----- DRAG INTERACTION
MENU::DragInteraction::DragInteraction(std::shared_ptr<MenuComponent> aParent)
	: Interaction(aParent)
{
	myType = InteractionType::Drag;
	myMin = 0.f;
	myMax = 100.f;
	myValue = 1.f;
}

void MENU::DragInteraction::OnPressed(const MenuUpdateContext& aContext)
{
	if (myParent->GetType() == ComponentType::Sprite)
	{
		std::shared_ptr<SpriteComponent> sprite = std::static_pointer_cast<SpriteComponent>(myParent);

		float desiredPosition = aContext.mousePosition.x;
		float screenPosition = sprite->GetPosition().x + sprite->GetParent().GetPosition().x;

		float movement = aContext.mouseDelta.x;
		if (movement < 0)
		{
			//LEFT
			if (desiredPosition < screenPosition)
				movement = 0.f;

		}
		else if (movement > 0)
		{
			//RIGHT
			if (desiredPosition > screenPosition)
				movement = 0.f;
		}

		//COMPARABLE VALUES!
		

		Vector2f newPosition = sprite->GetPosition();
		newPosition.x += movement;
		newPosition.x = std::clamp(newPosition.x, myMin, myMax);

		sprite->SetPosition(newPosition);
	}
}

// ----- HIDE INTERACTION
MENU::HideInteraction::HideInteraction(std::shared_ptr<MenuComponent> aParent)
	: Interaction(aParent)
{
	myType = InteractionType::Hide;
	myIsHidden = false;
}

void MENU::HideInteraction::OnPressed(const MenuUpdateContext& aContext)
{
	if (!aContext.mouseReleased)
		return;

	if (myParent->GetType() == ComponentType::Sprite)
	{
		std::shared_ptr<SpriteComponent> sprite = std::static_pointer_cast<SpriteComponent>(myParent);

		myIsHidden = !sprite->GetIsHidden();
		sprite->SetIsHidden(myIsHidden);
	}

	if (myParent->GetType() == ComponentType::Text)
	{
		std::shared_ptr<TextComponent> text = std::static_pointer_cast<TextComponent>(myParent);

		myIsHidden = !text->GetIsHidden();
		text->SetIsHidden(myIsHidden);
	}
}

// ----- CLIP INTERACTION
MENU::ClipInteraction::ClipInteraction(std::shared_ptr<MenuComponent> aParent)
	: Interaction(aParent)
{
	myType = InteractionType::Clip;
	myValue = 1.f;
}

void MENU::ClipInteraction::OnPressed(const MenuUpdateContext& aContext)
{
	if (aContext.mouseDelta.x == 0.f)
		return;

	if (myParent->GetType() == ComponentType::Sprite)
	{
		std::shared_ptr<SpriteComponent> sprite = std::static_pointer_cast<SpriteComponent>(myParent);

		myValue = std::clamp((aContext.mouseDelta.x / 100.f) * -1.f, 0.f, 1.f);

		ClipValue clip;
		clip.right = myValue;

		sprite->SetClipValue(clip);
	}
}
