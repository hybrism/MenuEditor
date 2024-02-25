#include "SpriteComponent.h"
#include <engine/graphics/GraphicsEngine.h>
#include <engine/graphics/sprite/SpriteDrawer.h>

MENU::SpriteComponent::SpriteComponent(MenuObject& aParent)
	: MenuComponent(aParent, eComponentType::Sprite)
{}

void MENU::SpriteComponent::Update()
{
	myInstance.position = myParent.GetPosition() + myPosition;
}

void MENU::SpriteComponent::Render()
{
	if (!mySharedData.texture)
		return;

	//TODO: Don't do this for all components
	GraphicsEngine::GetInstance()->GetSpriteDrawer().Draw(mySharedData, myInstance);
}

Texture* MENU::SpriteComponent::GetTexture() const
{
	return mySharedData.texture;
}

Vector2f MENU::SpriteComponent::GetTextureSize() const
{
	if (mySharedData.texture == nullptr)
		return Vector2f(0, 0);

	return mySharedData.texture->GetTextureSize();
}

std::string MENU::SpriteComponent::GetTexturePath()
{
	if (mySharedData.texture == nullptr)
		return "";

	return myTextureFile;
}

void MENU::SpriteComponent::SetPosition(const Vector2f& aPosition)
{
	myPosition = aPosition;
}

void MENU::SpriteComponent::SetPivot(const Vector2f& aPivot)
{
	myInstance.pivot = aPivot;
}

void MENU::SpriteComponent::SetSize(const Vector2f& aPosition)
{
	myInstance.size = aPosition;
}

void MENU::SpriteComponent::SetScaleMultiplier(const Vector2f& aPosition)
{
	myInstance.scaleMultiplier = aPosition;
}

void MENU::SpriteComponent::SetColor(const Vector4f& aPosition)
{
	myInstance.color = aPosition;
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

void MENU::SpriteComponent::SetTexture(Texture* aTexture, const std::string& aTextureName)
{
	myTextureFile = aTextureName;
	mySharedData.texture = aTexture;
	myInstance.size = mySharedData.texture->GetTextureSize();
}
