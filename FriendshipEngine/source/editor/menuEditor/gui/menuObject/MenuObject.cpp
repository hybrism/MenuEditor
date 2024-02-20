#include "MenuObject.h"

ME::MenuObject::MenuObject(const unsigned int aID) : myID(aID) {}

void ME::MenuObject::Init()
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Init();
	}
}

void ME::MenuObject::Update()
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Update();
	}
}

void ME::MenuObject::Render()
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Render();
	}
}
