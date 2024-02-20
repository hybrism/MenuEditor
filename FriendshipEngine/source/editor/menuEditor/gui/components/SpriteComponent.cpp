#include "SpriteComponent.h"
#include <engine/utility/Error.h>
#include <engine/graphics/GraphicsEngine.h>
#include <engine/graphics/sprite/SpriteDrawer.h>

void ME::SpriteComponent::Init()
{
}

void ME::SpriteComponent::Update()
{
}

void ME::SpriteComponent::Render()
{
	if (!mySharedData.texture)
		return;

	//TODO: Don't do this for all components
	GraphicsEngine* ge = GraphicsEngine::GetInstance();
	SpriteDrawer& spriteDrawer = ge->GetSpriteDrawer();

	ge->PrepareForSpriteRender();
	spriteDrawer.Draw(mySharedData, myInstance);
}

Vector2f& ME::SpriteComponent::GetPosition()
{
	return myInstance.position;
}

Vector2f& ME::SpriteComponent::GetSize()
{
	return myInstance.scale;
}

Vector2f ME::SpriteComponent::GetTextureSize() const
{
	if (mySharedData.texture == nullptr)
		return Vector2f(0, 0);

	return mySharedData.texture->GetTextureSize();
}

void ME::SpriteComponent::SetTexture(Texture* aTexture)
{
	mySharedData.texture = aTexture;
	myInstance.scale = mySharedData.texture->GetTextureSize();
}

void ME::SpriteComponent::SetPosition(Vector2f aPosition)
{
	myInstance.position = aPosition;
}
