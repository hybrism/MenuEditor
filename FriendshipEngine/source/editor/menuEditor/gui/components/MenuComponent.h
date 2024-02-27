#pragma once
#include "../MenuObject.h"

namespace MENU
{
	class MenuComponent
	{
	public:
		virtual void Update() {};
		virtual void Render() {};

		MenuObject& GetParent() const { return myParent; }
		ComponentType GetType() const { return myType; }
		Vector2f& GetPosition() { return myPosition; }

		virtual void UpdatePosition() {};

	protected:
		MenuComponent(MenuObject& aParent, ComponentType aType)
			: myParent(aParent)
			, myType(aType)
			, myPosition({ 0.f,0.f })
		{}

		virtual ~MenuComponent() {}

		MenuObject& myParent;
		const ComponentType myType;
		Vector2f myPosition;
	};
}