#pragma once
#include "MenuComponent.h"

namespace MENU
{
	class MenuObject;

	class Collider2DComponent : public MenuComponent
	{
	public:
		Collider2DComponent(MenuObject& aParent);

		void Update() override;
		void Render() override;

		void UpdatePosition();

		bool CheckCollision(const Vector2f& aPosition);
		bool IsHovered() { return myIsHovered; }

		Vector2f GetPosition() const { return myPosition; }
		Vector2f GetSize() const { return mySize; }
		bool GetShouldRenderColliders() const { return myShouldRenderColliders; }

		void SetPosition(const Vector2f& aPosition);
		void SetSize(const Vector2f& aSize);
		void SetShouldRenderColliders(bool aShouldRender);

	private:
		Vector2f myMin;
		Vector2f myMax;
		Vector2f mySize;

		bool myIsHovered;
		bool myShouldRenderColliders;

		void UpdateMinMax();
		void RenderColliders();
	};
}