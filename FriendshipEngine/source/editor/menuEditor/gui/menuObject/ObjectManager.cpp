#include "ObjectManager.h"
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
        myObjects[i].Update();
    }
}

void ObjectManager::Render()
{
    for (size_t i = 0; i < myObjects.size(); i++)
    {
        myObjects[i].Render();
    }
}

MenuObject& ObjectManager::CreateNew()
{
    myObjects.push_back(MenuObject(myIdCounter));
    
    myIdCounter++;
    myLastObjectIndex = myObjects.size() - 1;

    return myObjects[myLastObjectIndex];
}

void ObjectManager::ClearAll()
{
    myObjects.clear();
    myIdCounter = 0;
    myLastObjectIndex = 0;
}
