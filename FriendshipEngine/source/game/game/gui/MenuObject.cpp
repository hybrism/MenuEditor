#include "pch.h"
#include "MenuObject.h"
#include "components/MenuComponent.h"

MenuObject::MenuObject(const unsigned int aID)
	: myID(aID)
	, myName("untitled")
	, myPosition(100.f, 100.f) //TODO: Set to screen center
{
}

void MenuObject::Init()
{
	for (int i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Init();
	}
}

void MenuObject::Update()
{
	for (int i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Update();
	}
}

void MenuObject::Render()
{
	for (int i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Render();
	}
}
