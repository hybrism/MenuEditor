#pragma once
#include "../MenuUpdateContext.h"
#include "ComponentTypeEnum.h"

namespace MENU
{
	class MenuObject;
	class MenuComponent
	{

	public:
		virtual void Update(const MenuUpdateContext& aContext) { aContext; }
		virtual void Render() { __noop; };
		virtual void OnResize(const Vector2f& aScale) { aScale; };

		MenuObject& GetParent() const { return myParent; }
		ComponentType GetType() const { return myType; }
		Vector2f& GetPosition() { return myPosition; }
		const unsigned int GetID() const { return myID; }

		virtual void UpdatePosition() {};

	protected:
		MenuComponent(MenuObject& aParent, unsigned int aID, ComponentType aType)
			: myParent(aParent)
			, myType(aType)
			, myPosition({ 0.f,0.f })
			, myID(aID)
			, myName("")
		{}

		virtual ~MenuComponent() {}

		MenuObject& myParent;
		const ComponentType myType;
		const unsigned int myID;
		Vector2f myPosition;
		Vector2f myInitialPosition;

	public:
		std::string myName;
	};
}