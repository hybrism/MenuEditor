#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cassert>

#include "IDManager.h"
#include "MenuUpdateContext.h"

#include "components/ComponentTypeEnum.h"
#include "components/MenuComponent.h"

namespace MENU
{
	class MenuObject
	{
		friend class ObjectManager;

	public:
		MenuObject(const unsigned int aID, const Vector2f& aPosition = { 0.f, 0.f });
		~MenuObject();

		template<class T>
		T& AddComponent();

		void AddComponentOfType(ComponentType aType);

		template<class T>
		T& GetComponent();

		template<class T>
		std::vector<std::shared_ptr<MenuComponent>> GetComponents();

		void RemoveComponent(const ID aID);

		template<class T>
		bool HasComponent();

		virtual void Update(const MenuUpdateContext& aContext);
		virtual void Render();

		void OnResize(const Vector2i& aNewRenderSize);

		bool IsHovered();
		bool IsPressed();

		void SetName(const std::string& aName) { myName = aName; }
		void SetPosition(const Vector2f& aPosition, bool aIsInitialPosition = false);

		const ID GetID() const { return myID; }
		const ObjectState GetState() { return myState; }
		std::string& GetName() { return myName; }
		Vector2f GetPosition() const { return myPosition; }

	private:
		MenuObject() = delete;

		std::string myName;
		std::vector<std::shared_ptr<MenuComponent>> myComponents;

		Vector2i myTargetRenderSize;
		Vector2f myInitialPosition;
		Vector2f myPosition;
		ObjectState myState;
		const ID myID;

		ID myComponentIDCounter = 0;
	};
}

template<class T>
inline T& MENU::MenuObject::AddComponent()
{
	std::shared_ptr<T> component = std::make_shared<T>(*this, myComponentIDCounter++);
	myComponents.emplace_back(component);
	return *component;
}

/// <summary>
/// Gets reference to first component of type
/// </summary>
template<class T>
inline T& MENU::MenuObject::GetComponent()
{
	std::shared_ptr<MenuComponent> result = nullptr;

	for (size_t i = 0; i < myComponents.size(); i++)
	{
		if (typeid(*myComponents[i]) == typeid(T))
		{
			result = myComponents[i];
		}
	}

	assert(result != nullptr && "Trying to get component that does not exist!");

	return static_cast<T&>(*result);
}

/// <summary>
/// Gets vector of pointers to all components of type
/// </summary>
template<class T>
inline std::vector<std::shared_ptr<MENU::MenuComponent>> MENU::MenuObject::GetComponents()
{
	std::vector<std::shared_ptr<MenuComponent>> result;

	for (size_t i = 0; i < myComponents.size(); i++)
	{
		if (typeid(*myComponents[i]) == typeid(T))
		{
			result.push_back(myComponents[i]);
		}
	}

	return result;
}

template<class T>
inline bool MENU::MenuObject::HasComponent()
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		if (typeid(*myComponents[i]) == typeid(T))
			return true;
	}

	return false;
}
