#include "pch.h"

#include "ObjectManager.h"
#include "MenuObject.h"

ObjectManager::ObjectManager()
{
    myObjectIdCounter = 0;
    myLastObjectIndex = 0;
}

void ObjectManager::Update()
{
    for (size_t i = 0; i < myObjects.size(); i++)
    {
        PrintI("ObjectManager Update!")
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

MenuObject& ObjectManager::CreateNew()
{
    myObjects.push_back(std::make_shared<MenuObject>(myObjectIdCounter));
    
    myObjectIdCounter++;
    myLastObjectIndex = myObjects.size() - 1;

    return *myObjects[myLastObjectIndex];
}

void ObjectManager::ClearAll()
{
    myObjects.clear();
    myObjectIdCounter = 0;
    myLastObjectIndex = 0;
}
