#include "TextComponent.h"
#include "../MenuObject.h"
#include <engine/utility/StringHelper.h>

MENU::TextComponent::TextComponent(MenuObject& aParent, unsigned int aID)
	: MenuComponent(aParent, aID, ComponentType::Text)
{
	myString = "(None)";
	myFontName = "ProggyClean.ttf";
	mySize = eSize::Small;
	myIsCentered = false;

	UpdateText();

	myText.SetPosition(myParent.GetPosition());
}

void MENU::TextComponent::Update(const MenuUpdateContext& aContext)
{
	aContext;
}

void MENU::TextComponent::Render()
{
	myText.Render();
}

void MENU::TextComponent::UpdatePosition()
{
	if (myIsCentered)
		myText.CenterTextOverPosition(myParent.GetPosition() + myPosition);
	else
		myText.SetPosition(myParent.GetPosition() + myPosition);
}

std::string MENU::TextComponent::GetText()
{
	return myString;
}

std::string MENU::TextComponent::GetFontName()
{
	return myFontName;
}

MENU::eSize MENU::TextComponent::GetFontSize()
{
	return mySize;
}

const Vector2f MENU::TextComponent::GetPosition()
{
	return myPosition;
}

const Vector4f MENU::TextComponent::GetColor()
{
	return myText.GetColor();
}

bool MENU::TextComponent::GetIsCentered() const
{
	return myIsCentered;
}

void MENU::TextComponent::UpdateText()
{
	std::wstring wstr = StringHelper::s2ws(myFontName);
	const wchar_t* fontFile = wstr.c_str();

	Vector2f pos = myPosition;
	Vector4f color = myText.GetColor();

	myText = Text(fontFile, SizeAsFontSize());

	myText.SetText(myString);
	myText.SetColor(color);
	SetPosition(pos);
}

FontSize MENU::TextComponent::SizeAsFontSize()
{
	switch (mySize)
	{
	case MENU::eSize::Small:
		return FontSize_10;
	case MENU::eSize::Medium:
		return FontSize_24;
	case MENU::eSize::Large:
		return FontSize_36;
	default:
		break;
	}

	return FontSize_10;
}

void MENU::TextComponent::SetFont(const std::string& aFontName)
{
	myFontName = aFontName;
	UpdateText();
}

void MENU::TextComponent::SetFontSize(eSize aSize)
{
	mySize = aSize;
	UpdateText();
}

void MENU::TextComponent::SetText(const std::string& aText)
{
	myString = aText;
	myText.SetText(aText);
}

void MENU::TextComponent::SetPosition(const Vector2f& aPosition)
{
	myPosition = aPosition;

	UpdatePosition();
}

void MENU::TextComponent::SetColor(const Vector4f& aColor)
{
	myText.SetColor(aColor);
}

void MENU::TextComponent::SetIsCentered(bool aBool)
{
	myIsCentered = aBool;

	UpdatePosition();
}

void MENU::TextComponent::CenterTextOverPosition()
{
	myText.CenterTextOverPosition(myParent.GetPosition() + myPosition);
}
