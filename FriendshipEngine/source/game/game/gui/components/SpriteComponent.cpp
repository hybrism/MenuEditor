#include "pch.h"
#include "SpriteComponent.h"

#include <engine/graphics/sprite/SpriteDrawer.h>

void SpriteComponent::Init()
{
}

void SpriteComponent::Update()
{
}

void SpriteComponent::Render()
{
	if (!mySharedData.texture)
		return;

	//TODO: Don't do this for all components
	SpriteDrawer& spriteDrawer = GraphicsEngine::GetInstance()->GetSpriteDrawer();
	spriteDrawer.Draw(mySharedData, myInstance);
}

SpriteInstanceData& SpriteComponent::GetInstanceData()
{
	return myInstance;
}

Vector2f& SpriteComponent::GetPosition()
{
	return myInstance.position;
}

Vector2f& SpriteComponent::GetSize()
{
	return myInstance.size;
}

Vector2f& SpriteComponent::GetPivot()
{
	return myInstance.pivot;
}

Vector2f& SpriteComponent::GetScaleMultiplier()
{
	return myInstance.scaleMultiplier;
}

Vector4f& SpriteComponent::GetColor()
{
	return myInstance.color;
}

ClipValue& SpriteComponent::GetClipValue()
{
	return myInstance.clip;
}

float& SpriteComponent::GetRotation()
{
	return myInstance.rotation;
}

bool& SpriteComponent::GetIsHidden()
{
	return myInstance.isHidden;
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

void SpriteComponent::SetTexture(Texture* aTexture, const std::string& aTextureName)
{
	myTextureFile = aTextureName;
	mySharedData.texture = aTexture;
	myInstance.size = mySharedData.texture->GetTextureSize();
}

void SpriteComponent::SetPosition(Vector2f aPosition)
{
	myInstance.position = aPosition;
}