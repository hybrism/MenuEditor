#pragma once
#include <string>
#include <vector>
#include <memory>
#include <engine/math/Vector.h>

class MenuComponent;
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
	bool HasComponent();

	virtual void Init();
	virtual void Update();
	virtual void Render();

	void SetName(const std::string& aName) { myName = aName; }
	void SetPosition(const Vector2f& aPosition) { myPosition = aPosition; }

	const unsigned int GetID() const { return myID; }
	std::string GetName() const { return myName; }
	Vector2f GetPosition() const { return myPosition; }

private:
	MenuObject() = delete;

	std::vector<std::shared_ptr<MenuComponent>> myComponents;

	const unsigned int myID;
	std::string myName;
	Vector2f myPosition;
};

template<class T>
inline T& MenuObject::AddComponent()
{
	std::shared_ptr<T> component = std::make_shared<T>(*this);
	myComponents.emplace_back(component);
	return *component;
}

template<class T>
inline T& MenuObject::GetComponent()
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
inline bool MenuObject::HasComponent()
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		if (typeid(*myComponents[i]) == typeid(T))
			return true;
	}

	return false;
}
