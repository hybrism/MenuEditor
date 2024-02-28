#include "Collider2DComponent.h"
#include "../MenuObject.h"
#include <engine/graphics/GraphicsEngine.h>
#include <engine/utility/Error.h>

MENU::Collider2DComponent::Collider2DComponent(MenuObject& aParent, unsigned int aID)
	: MenuComponent(aParent, aID, ComponentType::Collider2D)
	, myMin({ 0.f,0.f })
	, myMax({ 100.f,100.f })
	, mySize({ 50.f,50.f })
	, myIsHovered(false)
	, myShouldRenderColliders(true)
{
	UpdateMinMax();
}

void MENU::Collider2DComponent::Update()
{
	//UpdateMinMax();
}

void MENU::Collider2DComponent::Render()
{
	if (myShouldRenderColliders)
		RenderColliders();
}

void MENU::Collider2DComponent::UpdatePosition()
{
	UpdateMinMax();
}

void MENU::Collider2DComponent::RenderColliders()
{
	auto ge = GraphicsEngine::GetInstance();
	Vector2i renderSize = ge->GetViewportDimensions();
	DebugRenderer& debug = ge->GetDebugRenderer();

	//Flip Y
	Vector2f min = { myMin.x, (float)renderSize.y - myMin.y };
	Vector2f max = { myMax.x, (float)renderSize.y - myMax.y };

	debug.DrawLine({ min.x, min.y }, { max.x, min.y });
	debug.DrawLine({ max.x, min.y }, { max.x, max.y });
	debug.DrawLine({ max.x, max.y }, { min.x, max.y });
	debug.DrawLine({ min.x, max.y }, { min.x, min.y });
}

bool MENU::Collider2DComponent::CheckCollision(const Vector2f& aPosition)
{
	bool min = aPosition.x > myMin.x && aPosition.y > myMin.y;
	bool max = aPosition.x < myMax.x && aPosition.y < myMax.y;

	myIsHovered = min && max;

	return myIsHovered;
}

void MENU::Collider2DComponent::SetShouldRenderColliders(bool aShouldRender)
{
	myShouldRenderColliders = aShouldRender;
}

void MENU::Collider2DComponent::SetPosition(const Vector2f& aPosition)
{
	myPosition = aPosition;
	UpdateMinMax();
}

void MENU::Collider2DComponent::SetSize(const Vector2f& aSize)
{
	mySize = aSize;
	UpdateMinMax();
}

void MENU::Collider2DComponent::UpdateMinMax()
{
	myMin = myParent.GetPosition() + myPosition - (mySize * 0.5f);
	myMax = myParent.GetPosition() + myPosition + (mySize * 0.5f);
}
