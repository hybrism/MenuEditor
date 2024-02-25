#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <engine/math/Vector.h>
#include "../MenuCommon.h"

namespace MENU
{
	class MenuComponent;

	enum class eComponentType
	{
		Sprite,
		Collider2D,
		Text,
		Count
	};

	class MenuObject
	{
		friend class ObjectManager;

	public:

		MenuObject(const unsigned int aID, const Vector2f& aPosition = { 0.f, 0.f });

		template<class T>
		T& AddComponent();

		template<class T>
		T& GetComponent();

		void AddComponentOfType(eComponentType aType);

		template<class T>
		bool HasComponent();

		virtual void Init();
		virtual void Update();
		virtual void Render();

		//TODO: When setposition on ParentObject->UpdateChildComponents
		void SetName(const std::string& aName) { myName = aName; }
		void SetPosition(const Vector2f& aPosition) { myPosition = aPosition; }

		const unsigned int GetID() const { return myID; }
		std::string GetName() const { return myName; }
		Vector2f GetPosition() const { return myPosition; }

	private:
		MenuObject() = delete;

		//TODO: Store components as a map with vectors, so we can have more of the same
		std::vector<std::shared_ptr<MenuComponent>> myComponents;

		const unsigned int myID;
		std::string myName;
		Vector2f myPosition;
	};
}

template<class T>
inline T& MENU::MenuObject::AddComponent()
{
	std::shared_ptr<T> component = std::make_shared<T>(*this);
	myComponents.emplace_back(component);
	return *component;
}

template<class T>
inline T& MENU::MenuObject::GetComponent()
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		if (typeid(*myComponents[i]) == typeid(T))
		{
			return static_cast<T&>(*myComponents[i]);
		}
	}

	//TODO: Fix this
	T& fail = static_cast<T&>(*myComponents[0]);
	return fail;
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
