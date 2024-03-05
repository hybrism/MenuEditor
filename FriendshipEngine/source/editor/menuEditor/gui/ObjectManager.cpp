#include "ObjectManager.h"
#include <engine/utility/Error.h>
#include <memory>

#include "components/Collider2DComponent.h"

#include "MenuObject.h"

MENU::ObjectManager::ObjectManager()
{
	myObjectIdCounter = 0;
	myLastObjectIndex = 0;
}

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
		if (myObjects[i]->HasComponent<Collider2DComponent>())
		{
			Collider2DComponent& collider = myObjects[i]->GetComponent<Collider2DComponent>();

			collider.CheckCollision(aPosition, aIsPressed);
		}
	}
}

MENU::MenuObject& MENU::ObjectManager::CreateNew(unsigned int aID, const Vector2f& aPosition)
{
	if (aID == UINT_MAX)
	{
		myObjects.push_back(std::make_shared<MenuObject>(myObjectIdCounter, aPosition));

		myObjectIdCounter++;
	}
	else
	{
		myObjects.push_back(std::make_shared<MenuObject>(aID, aPosition));
		myObjectIdCounter = aID + 1;
	}

	myLastObjectIndex = myObjects.size() - 1;

	return *myObjects[myLastObjectIndex];
}

MENU::MenuObject& MENU::ObjectManager::GetObjectFromID(unsigned int aID)
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

void MENU::ObjectManager::RemoveObjectAtID(unsigned int aID)
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
	myObjectIdCounter--;
	myLastObjectIndex = myObjects.size() - 1;
}

void MENU::ObjectManager::ClearAll()
{
	myObjects.clear();
	myObjectIdCounter = 0;
	myLastObjectIndex = 0;
}
