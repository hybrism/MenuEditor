#include "ObjectManager.h"
#include <engine/utility/Error.h>

#include "components/Collider2DComponent.h"

#include "IDManager.h"
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
		if (myObjects[i]->HasComponent<Collider2DComponent>())
		{
			Collider2DComponent& collider = myObjects[i]->GetComponent<Collider2DComponent>();

			collider.CheckCollision(aPosition, aIsPressed);
		}
	}
}

MENU::MenuObject& MENU::ObjectManager::CreateNew(unsigned int aID, const Vector2f& aPosition)
{
	IDManager* idManager = IDManager::GetInstance();

	if (aID == UINT_MAX)
	{
		myObjects.push_back(std::make_shared<MenuObject>(idManager->GetFreeID(), aPosition));
	}
	else
	{
		myObjects.push_back(std::make_shared<MenuObject>(aID, aPosition));
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
	myLastObjectIndex = myObjects.size() - 1;
}

void MENU::ObjectManager::ClearAll()
{
	IDManager* idManager = IDManager::GetInstance();
	for (unsigned int i = 0; i < myObjects.size(); i++)
	{
		idManager->FreeID(myObjects[i]->GetID());
	}

	myObjects.clear();
	myLastObjectIndex = 0;
}
