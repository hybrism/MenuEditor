#include "MenuObject.h"
#include "components/MenuComponent.h"

MenuObject::MenuObject(const unsigned int aID, const Vector2f& aPosition)
	: myID(aID)
	, myName("untitled")
	, myPosition(aPosition)
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
