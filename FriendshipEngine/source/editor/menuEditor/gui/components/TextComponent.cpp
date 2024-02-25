#include "TextComponent.h"
#include <engine/text/text.h>

MENU::TextComponent::TextComponent(MenuObject& aParent)
	: MenuComponent(aParent, eComponentType::Text)
{
	myText = new Text(L"ProggyClean.ttf", FontSize_30);
	myText->SetPosition({0, 0});
}

void MENU::TextComponent::Update()
{

	myText->SetPosition(myParent.GetPosition() + myPosition);
}

void MENU::TextComponent::Render()
{
	if (myText)
		myText->Render();
}

std::string MENU::TextComponent::GetText()
{
	return myString;
}

const Vector2f MENU::TextComponent::GetPosition()
{
	return myPosition;
}

const Vector4f MENU::TextComponent::GetColor()
{
	return myText->GetColor();
}

void MENU::TextComponent::SetText(const std::string& aText)
{
	myString = aText;
	myText->SetText(aText);
}

void MENU::TextComponent::SetPosition(const Vector2f& aPosition)
{
	myPosition = aPosition;
}

void MENU::TextComponent::SetColor(const Vector4f& aColor)
{
	myText->SetColor(aColor);
}