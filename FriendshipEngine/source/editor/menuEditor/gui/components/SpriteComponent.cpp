#include "SpriteComponent.h"
#include <engine/graphics/GraphicsEngine.h>
#include <engine/graphics/sprite/SpriteDrawer.h>

SpriteComponent::SpriteComponent(MenuObject& aParent) 
	: MenuComponent(aParent, eComponentType::Sprite)
{}

void SpriteComponent::Init()
{
}

void SpriteComponent::Update()
{
	myInstance.position = myParent.GetPosition() + myPosition;
}

void SpriteComponent::Render()
{
	if (!mySharedData.texture)
		return;

	//TODO: Don't do this for all components
	GraphicsEngine::GetInstance()->GetSpriteDrawer().Draw(mySharedData, myInstance);
}

Texture* SpriteComponent::GetTexture() const
{
	return mySharedData.texture;
}

Vector2f SpriteComponent::GetTextureSize() const
{
	if (mySharedData.texture == nullptr)
		return Vector2f(0, 0);

	return mySharedData.texture->GetTextureSize();
}

std::string SpriteComponent::GetTexturePath()
{
	if (mySharedData.texture == nullptr)
		return "";

	return myTextureFile;
}

void SpriteComponent::SetPosition(const Vector2f& aPosition)
{
	myPosition = aPosition;
}

void SpriteComponent::SetPivot(const Vector2f& aPivot)
{
	myInstance.pivot = aPivot;
}

void SpriteComponent::SetSize(const Vector2f& aPosition)
{
	myInstance.size = aPosition;
}

void SpriteComponent::SetScaleMultiplier(const Vector2f& aPosition)
{
	myInstance.scaleMultiplier = aPosition;
}

void SpriteComponent::SetColor(const Vector4f& aPosition)
{
	myInstance.color = aPosition;
}

void SpriteComponent::SetClipValue(const ClipValue& aClipValue)
{
	myInstance.clip = aClipValue;
}

void SpriteComponent::SetRotation(float aRotation)
{
	myInstance.rotation = aRotation;
}

void SpriteComponent::SetIsHidden(bool aIsHidden)
{
	myInstance.isHidden = aIsHidden;
}

void SpriteComponent::SetTexture(Texture* aTexture, const std::string& aTextureName)
{
	myTextureFile = aTextureName;
	mySharedData.texture = aTexture;
	myInstance.size = mySharedData.texture->GetTextureSize();
}
