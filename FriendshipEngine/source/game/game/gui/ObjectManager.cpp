#include "pch.h"

#include "ObjectManager.h"
#include <engine/utility/Error.h>

#include "components/Collider2DComponent.h"

#include "MenuObject.h"

MENU::ObjectManager::ObjectManager()
{}

void MENU::ObjectManager::Update(const MenuUpdateContext& aContext)
{
	for (size_t i = 0; i < myObjects.size(); i++)
	{
		myObjects[i]->Update(aContext);
	}
}

void MENU::ObjectManager::Render()
{
	for (size_t i = 0; i < myObjects.size(); i++)
	{
		myObjects[i]->Render();
	}
}

void MENU::ObjectManager::CheckCollision(const Vector2f& aPosition, bool aIsPressed)
{
	for (size_t i = 0; i < myObjects.size(); i++)
	{
		if (!myObjects[i]->HasComponent<Collider2DComponent>())
			return;

		myObjects[i]->GetComponent<Collider2DComponent>().CheckCollision(aPosition, aIsPressed);
	}
}

MENU::MenuObject& MENU::ObjectManager::CreateNew(ID aID, const Vector2f& aPosition)
{
	assert(aID != INVALID_ID && "ID is not valid!");

	myObjects.push_back(std::make_shared<MenuObject>(aID, aPosition));

	myLastObjectIndex = myObjects.size() - 1;
	return *myObjects[myLastObjectIndex];
}

MENU::MenuObject& MENU::ObjectManager::GetObjectFromID(ID aID)
{
	for (size_t i = 0; i < myObjects.size(); i++)
	{
		if (myObjects[i]->GetID() == aID)
			return *myObjects[i];
	}

	assert("No object with this ID exists!");
	return *myObjects.front();
}

MENU::MenuObject& MENU::ObjectManager::GetObjectFromIndex(unsigned int aIndex)
{
	assert(aIndex <= myLastObjectIndex && "Index is out of range!");

	return *myObjects[aIndex];
}

void MENU::ObjectManager::RemoveObjectAtID(ID aID)
{
	for (unsigned int i = 0; i < myObjects.size(); i++)
	{
		unsigned int ID = myObjects[i]->GetID();
		if (ID == aID)
		{
			RemoveObjectAtIndex(i);
			return;
		}
	}
}

void MENU::ObjectManager::RemoveObjectAtIndex(unsigned int aIndex)
{
	assert(aIndex <= myLastObjectIndex && "Index is out of range!");

	myObjects.erase(myObjects.begin() + aIndex);
	myLastObjectIndex = myObjects.size() - 1;
}

void MENU::ObjectManager::ClearAll()
{
	myObjects.clear();
	myLastObjectIndex = 0;
}
