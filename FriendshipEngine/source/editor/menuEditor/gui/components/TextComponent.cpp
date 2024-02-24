#include "TextComponent.h"
#include <engine/text/text.h>

TextComponent::TextComponent(MenuObject& aParent)
	: MenuComponent(aParent, eComponentType::Text)
{
	myText = new Text(L"ProggyClean.ttf", FontSize_30);
	myText->SetPosition({0, 0});
}

void TextComponent::Init()
{
}

void TextComponent::Update()
{

	myText->SetPosition(myParent.GetPosition() + myPosition);
}

void TextComponent::Render()
{
	if (myText)
		myText->Render();
}

std::string TextComponent::GetText()
{
	return myString;
}

const Vector2f TextComponent::GetPosition()
{
	return myPosition;
}

const Vector4f TextComponent::GetColor()
{
	return myText->GetColor();
}

void TextComponent::SetText(const std::string& aText)
{
	myString = aText;
	myText->SetText(aText);
}

void TextComponent::SetPosition(const Vector2f& aPosition)
{
	myPosition = aPosition;
}

void TextComponent::SetColor(const Vector4f& aColor)
{
	myText->SetColor(aColor);
}