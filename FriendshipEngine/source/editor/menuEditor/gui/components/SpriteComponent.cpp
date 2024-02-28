#include "SpriteComponent.h"
#include "../MenuObject.h"

#include <engine/graphics/GraphicsEngine.h>
#include <engine/graphics/sprite/SpriteDrawer.h>

MENU::SpriteComponent::SpriteComponent(MenuObject& aParent, unsigned int aID)
	: MenuComponent(aParent, aID, ComponentType::Sprite)
{
	myState = TextureState::Default;
}

void MENU::SpriteComponent::Update()
{
	myInstance.position = myParent.GetPosition() + myPosition;

	myState = TextureState::Default;

	if (myParent.IsHovered())
		myState = TextureState::Hovered;
}

void MENU::SpriteComponent::Render()
{
	if (!myTextures[(int)myState].shared.texture)
		return;

	myInstance.color = myTextures[(int)myState].color;

	//TODO: Don't do this for all components
	GraphicsEngine::GetInstance()->GetSpriteDrawer().Draw(myTextures[(int)myState].shared, myInstance);
}

Texture* MENU::SpriteComponent::GetTexture(TextureState aType) const
{
	return myTextures[(int)aType].shared.texture;
}

Vector4f& MENU::SpriteComponent::GetColor(TextureState aType)
{
	return myTextures[(int)aType].color;
}

Vector2f MENU::SpriteComponent::GetTextureSize(TextureState aType) const
{
	if (myTextures[(int)aType].shared.texture == nullptr)
		return Vector2f(0, 0);

	return myTextures[(int)aType].shared.texture->GetTextureSize();
}

std::string MENU::SpriteComponent::GetTexturePath(TextureState aType)
{
	if (myTextures[(int)aType].shared.texture == nullptr)
		return "(None)";

	return myTextures[(int)aType].fileName;
}

void MENU::SpriteComponent::SetPosition(const Vector2f& aPosition)
{
	myPosition = aPosition;
}

void MENU::SpriteComponent::SetPivot(const Vector2f& aPivot)
{
	myInstance.pivot = aPivot;
}

void MENU::SpriteComponent::SetSize(const Vector2f& aSize)
{
	myInstance.size = aSize;
}

void MENU::SpriteComponent::SetScaleMultiplier(const Vector2f& aScaleMultiplier)
{
	myInstance.scaleMultiplier = aScaleMultiplier;
}

void MENU::SpriteComponent::SetColor(const Vector4f& aColor, TextureState aState)
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

void MENU::SpriteComponent::SetTexture(Texture* aTexture, const std::string& aTextureName, TextureState aType)
{
	myTextures[(int)aType].fileName = aTextureName;
	myTextures[(int)aType].shared.texture = aTexture;

	if (aType == TextureState::Default)
		myInstance.size = myTextures[(int)aType].shared.texture->GetTextureSize();
}
