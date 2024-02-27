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

void MENU::ObjectManager::Update()
{
	for (size_t i = 0; i < myObjects.size(); i++)
	{
		myObjects[i]->Update();
	}
}

void MENU::ObjectManager::Render()
{
	for (size_t i = 0; i < myObjects.size(); i++)
	{
		myObjects[i]->Render();
	}
}

void MENU::ObjectManager::CheckCollision(const Vector2f& aPosition)
{
	for (size_t i = 0; i < myObjects.size(); i++)
	{
		if (myObjects[i]->HasComponent<Collider2DComponent>())
		{
			Collider2DComponent& collider = myObjects[i]->GetComponent<Collider2DComponent>();
			collider.CheckCollision(aPosition);
		}
	}
}

MENU::MenuObject& MENU::ObjectManager::CreateNew(const Vector2f& aPosition)
{
	myObjects.push_back(std::make_shared<MenuObject>(myObjectIdCounter, aPosition));

	myObjectIdCounter++;
	myLastObjectIndex = myObjects.size() - 1;

	return *myObjects[myLastObjectIndex];
}

MENU::MenuObject& MENU::ObjectManager::GetObjectFromID(size_t aID)
{
	return *myObjects[aID];
}

void MENU::ObjectManager::ClearAll()
{
	myObjects.clear();
	myObjectIdCounter = 0;
	myLastObjectIndex = 0;
}
