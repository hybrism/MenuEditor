#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <engine/math/Vector.h>
#include "components/ComponentTypeEnum.h"

namespace MENU
{
	class MenuComponent;
	class SpriteComponent;
	class TextComponent;
	class Collider2DComponent;

	class MenuObject
	{
		friend class ObjectManager;

	public:

		MenuObject(const unsigned int aID, const Vector2f& aPosition = { 0.f, 0.f });

		template<class T>
		T& AddComponent();

		template<class T>
		T& GetComponent();

		template<class T>
		std::vector<std::shared_ptr<MenuComponent>> GetComponents();

		void AddComponentOfType(ComponentType aType);

		template<class T>
		bool HasComponent();

		virtual void Init();
		virtual void Update();
		virtual void Render();

		bool IsHovered();

		void SetName(const std::string& aName) { myName = aName; }
		void SetPosition(const Vector2f& aPosition);

		const unsigned int GetID() const { return myID; }
		std::string GetName() const { return myName; }
		Vector2f GetPosition() const { return myPosition; }

	private:
		MenuObject() = delete;

		std::vector<std::vector<std::shared_ptr<MenuComponent>>> myComponents;

		const unsigned int myID;
		std::string myName;
		Vector2f myPosition;
	};
}

template<class T>
inline T& MENU::MenuObject::AddComponent()
{
	std::shared_ptr<T> component = std::make_shared<T>(*this);

	for (size_t type = 0; type < myComponents.size(); type++)
	{
		assert(!myComponents[type].empty() && "This should never happen");

		if (typeid(T) == typeid(*myComponents[type].front()))
		{
			myComponents[type].emplace_back(component);
			return *component;
		}
	}

	std::vector<std::shared_ptr<MenuComponent>> newVec;
	newVec.emplace_back(component);
	myComponents.push_back(newVec);

	return *component;
}

/// <summary>
/// Gets reference to first component of type
/// </summary>
template<class T>
inline T& MENU::MenuObject::GetComponent()
{
	std::shared_ptr<MenuComponent> result = nullptr;

	for (size_t type = 0; type < myComponents.size(); type++)
	{
		assert(!myComponents[type].empty() && "This should never happen");

		if (typeid(*myComponents[type].front()) == typeid(T))
		{
			result = myComponents[type].front();
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
	for (size_t type = 0; type < myComponents.size(); type++)
	{
		assert(!myComponents[type].empty() && "This should never happen");

		if (typeid(*myComponents[type].front()) == typeid(T))
		{
			return myComponents[type];
		}
	}

	assert("Trying to get components that does not exist!");

	return std::vector<std::shared_ptr<MenuComponent>>();
}

template<class T>
inline bool MENU::MenuObject::HasComponent()
{
	for (size_t type = 0; type < myComponents.size(); type++)
	{
		assert(!myComponents[type].empty() && "This should never happen");

		if (typeid(*myComponents[type].front()) == typeid(T))
			return true;
	}

	return false;
}
