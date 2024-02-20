#pragma once
#include <vector>
#include <memory>

#include "../components/Component.h"

namespace ME
{
	class MenuObject
	{
		friend class ObjectManager;

	public:
		const unsigned int myID;

		MenuObject(const unsigned int aID);
		
		template<class ComponentType>
		ComponentType& AddComponent();

		template<class ComponentType>
		ComponentType& GetComponent();

		virtual void Init();
		virtual void Update();
		virtual void Render();

	private:
		std::vector<std::shared_ptr<Component>> myComponents;
	};

	template<class ComponentType>
	inline ComponentType& MenuObject::AddComponent()
	{
		std::shared_ptr<ComponentType> component = std::make_shared<ComponentType>(*this);
		myComponents.emplace_back(component);
		return *component;
	}

	template<class ComponentType>
	inline ComponentType& MenuObject::GetComponent()
	{
		for (size_t i = 0; i < myComponents.size(); i++)
		{
			if (myComponents[i] == ComponentType)
				return myComponents[i];
		}
	}
}