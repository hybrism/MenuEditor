#include "ObjectManager.h"
#include "MenuObject.h"

ME::ObjectManager::ObjectManager()
{
    myIdCounter = 0;
    myLastObjectIndex = 0;
}

void ME::ObjectManager::Update()
{
    for (size_t i = 0; i < myObjects.size(); i++)
    {
        myObjects[i].Update();
    }
}

void ME::ObjectManager::Render()
{
    for (size_t i = 0; i < myObjects.size(); i++)
    {
        myObjects[i].Render();
    }
}

ME::MenuObject& ME::ObjectManager::CreateNew()
{
    myObjects.push_back(MenuObject(myIdCounter));
    
    myIdCounter++;
    myLastObjectIndex = myObjects.size() - 1;

    return myObjects[myLastObjectIndex];
}
