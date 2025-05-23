#include "pch.h"

#include "SpriteComponent.h"
#include "../MenuObject.h"

#include <engine/graphics/GraphicsEngine.h>
#include <engine/graphics/sprite/SpriteDrawer.h>

MENU::SpriteComponent::SpriteComponent(MenuObject& aParent, unsigned int aID)
	: MenuComponent(aParent, aID, ComponentType::Sprite)

{}

void MENU::SpriteComponent::Update(const MenuUpdateContext& aContext)
{
	aContext;

	myInstance.position = myParent.GetPosition() + myPosition;
}

void MENU::SpriteComponent::Render()
{
	if (!myTextures[(int)myParent.GetState()].shared.texture)
		return;

	myInstance.color = myTextures[(int)myParent.GetState()].color;

	GraphicsEngine::GetInstance()->GetSpriteRenderer().DrawSprite(myTextures[(int)myParent.GetState()].shared, myInstance);
}

void MENU::SpriteComponent::OnResize(const Vector2f& aScale)
{
	myInstance.size = { aScale.x * myInitialSize.x, aScale.y * myInitialSize.y };
	myInstance.position = { aScale.x * myInitialPosition.x, aScale.y * myInitialPosition.y };
}

std::shared_ptr<Texture> MENU::SpriteComponent::GetTexture(ObjectState aType) const
{
	return myTextures[(int)aType].shared.texture;
}

Vector4f& MENU::SpriteComponent::GetColor(ObjectState aType)
{
	return myTextures[(int)aType].color;
}

Vector2f MENU::SpriteComponent::GetTextureSize(ObjectState aType) const
{
	if (myTextures[(int)aType].shared.texture == nullptr)
		return Vector2f(0, 0);

	return myTextures[(int)aType].shared.texture->GetTextureSize();
}

std::string MENU::SpriteComponent::GetTexturePath(ObjectState aType)
{
	if (myTextures[(int)aType].shared.texture == nullptr)
		return "(None)";

	return myTextures[(int)aType].fileName;
}

void MENU::SpriteComponent::SetPosition(const Vector2f& aPosition)
{
	myPosition = aPosition;
	myInitialPosition = aPosition;
}

void MENU::SpriteComponent::SetPivot(const Vector2f& aPivot)
{
	myInstance.pivot = aPivot;
}

void MENU::SpriteComponent::SetSize(const Vector2f& aSize)
{
	myInstance.size = aSize;
	myInitialSize = aSize;
}

void MENU::SpriteComponent::SetScaleMultiplier(const Vector2f& aScaleMultiplier)
{
	myInstance.scaleMultiplier = aScaleMultiplier;
}

void MENU::SpriteComponent::SetColor(const Vector4f& aColor, ObjectState aState)
{
	myTextures[(int)aState].color = aColor;
}

void MENU::SpriteComponent::SetClipValue(const ClipValue& aClipValue)
{
	myInstance.clip = aClipValue;
}

void MENU::SpriteComponent::SetRotation(float aRotation)
{
	myInstance.rotation = aRotation;
}

void MENU::SpriteComponent::SetIsHidden(bool aIsHidden)
{
	myInstance.isHidden = aIsHidden;
}

void MENU::SpriteComponent::SetTexture(std::shared_ptr<Texture> aTexture, const std::string& aTextureName, ObjectState aType)
{
	myTextures[(int)aType].fileName = aTextureName;
	myTextures[(int)aType].shared.texture = aTexture;

	if (aType == ObjectState::Default)
	{
		myInstance.size = myTextures[(int)aType].shared.texture->GetTextureSize();
		myInitialSize = myInstance.size;
	}
}
