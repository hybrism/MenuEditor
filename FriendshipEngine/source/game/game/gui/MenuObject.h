#pragma once
#include <engine/math/Vector.h>

class MenuComponent;
class MenuObject
{
	friend class ObjectManager;

public:
	const unsigned int myID;

	MenuObject(const unsigned int aID);

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

	std::string GetName() const { return myName; }
	Vector2f GetPosition() const { return myPosition; }

private:
	std::vector<std::shared_ptr<MenuComponent>> myComponents;
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
