#include "ObjectManager.h"
#include <engine/utility/Error.h>
#include <memory>

#include "MenuObject.h"

ObjectManager::ObjectManager()
{
    myIdCounter = 0;
    myLastObjectIndex = 0;
}

void ObjectManager::Update()
{
    for (size_t i = 0; i < myObjects.size(); i++)
    {
        myObjects[i]->Update();
    }
}

void ObjectManager::Render()
{
    for (size_t i = 0; i < myObjects.size(); i++)
    {
        myObjects[i]->Render();
    }
}

MenuObject& ObjectManager::CreateNew(const Vector2f& aPosition)
{
    myObjects.push_back(std::make_shared<MenuObject>(myIdCounter, aPosition));
    
    myIdCounter++;
    myLastObjectIndex = myObjects.size() - 1;

    return *myObjects[myLastObjectIndex];
}

void ObjectManager::ClearAll()
{
    myObjects.clear();
    myIdCounter = 0;
    myLastObjectIndex = 0;
}
