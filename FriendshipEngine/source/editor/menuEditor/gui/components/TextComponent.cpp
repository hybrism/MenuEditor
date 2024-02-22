#include "TextComponent.h"
#include "../menuObject/MenuObject.h"
#include <engine/utility/Error.h>
#include <engine/text/text.h>

TextComponent::TextComponent(MenuObject& aParent)
	: Component(aParent)
{
	myText = new Text(L"ProggyClean.ttf", FontSize_30);
	myText->SetPosition({100, 100});
}

void TextComponent::Init()
{
}

void TextComponent::Update()
{
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
	return myText->GetPosition();
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
	myText->SetPosition(aPosition);
}

void TextComponent::SetColor(const Vector4f& aColor)
{
	myText->SetColor(aColor);
}

