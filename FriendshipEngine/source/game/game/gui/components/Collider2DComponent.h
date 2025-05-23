#pragma once
#include "MenuComponent.h"

namespace MENU
{
	class MenuObject;

	class Collider2DComponent : public MenuComponent
	{
	public:
		Collider2DComponent(MenuObject& aParent, unsigned int aID);

		void Update(const MenuUpdateContext& aContext) override;
		void Render() override;

		void OnResize(const Vector2f& aScale) override;

		void UpdatePosition();

		bool CheckCollision(const MenuUpdateContext& aContext);
		bool IsHovered() { return myIsHovered; }
		bool IsPressed() { return myIsPressed; }

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
		Vector2f myInitialSize;

		bool myIsHovered;
		bool myIsPressed;

		bool myShouldRenderColliders;

		void UpdateMinMax();
		void RenderColliders();
	};
}