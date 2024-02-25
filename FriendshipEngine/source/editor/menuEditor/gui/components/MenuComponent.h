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
		eComponentType GetType() const { return myType; }
		Vector2f& GetPosition() { return myPosition; }

	protected:
		MenuComponent(MenuObject& aParent, eComponentType aType)
			: myParent(aParent)
			, myType(aType)
			, myPosition({ 0.f,0.f })
		{}

		virtual ~MenuComponent() {}

		MenuObject& myParent;
		const eComponentType myType;
		Vector2f myPosition;
	};
}