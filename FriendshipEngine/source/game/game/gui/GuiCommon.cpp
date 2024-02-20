#include "pch.h"
#include "GuiCommon.h"
#include <engine/graphics/sprite/SpriteDrawer.h>

void GuiSprite::Render(SpriteDrawer& aSpriteDrawer)
{
	aSpriteDrawer.Draw(mySharedData, myInstance);
}

Vector2f GuiSprite::GetTextureSize() const
{
	return mySharedData.texture->GetTextureSize();
}

bool GuiCollider::IsHovered(const Vector2f& aMousePosition)
{
	bool min = aMousePosition.x > myMin.x && aMousePosition.y > myMin.y;
	bool max = aMousePosition.x < myMax.x && aMousePosition.y < myMax.y;

	myIsHovered = min && max;

	return min && max;
}

void GuiCollider::ReadDataFromSprite(const GuiSprite& aSprite)
{
	mySize.x = aSprite.GetTextureSize().x * aSprite.myInstance.scaleMultiplier.x;
	mySize.y = aSprite.GetTextureSize().y * aSprite.myInstance.scaleMultiplier.y;
	myPosition = aSprite.myInstance.position;

	UpdateMinMax();
}

void GuiCollider::ReadDataFromSprite(const SpriteInstanceData& aSpriteInstance, const SpriteSharedData& aSharedData)
{
	mySize.x = aSharedData.texture->GetTextureSize().x * aSpriteInstance.scaleMultiplier.x;
	mySize.y = aSharedData.texture->GetTextureSize().y * aSpriteInstance.scaleMultiplier.y;
	myPosition = aSpriteInstance.position;

	UpdateMinMax();
}

void GuiCollider::SetSize(const Vector2f& aSize)
{
	mySize = aSize;
	UpdateMinMax();
}

void GuiCollider::SetPosition(const Vector2f& aPosition)
{
	myPosition = aPosition;
	UpdateMinMax();
}

void GuiCollider::UpdateMinMax()
{
	myMin = myPosition - (mySize * 0.5f);
	myMax = myPosition + (mySize * 0.5f);
}
