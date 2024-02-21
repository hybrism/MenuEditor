#include "MenuObject.h"

MenuObject::MenuObject(const unsigned int aID) 
	: myID(aID)
	, myName("untitled")
{}

void MenuObject::Init()
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Init();
	}
}

void MenuObject::Update()
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Update();
	}
}

void MenuObject::Render()
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Render();
	}
}
