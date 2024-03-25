#include "pch.h"

#include "TextComponent.h"
#include "../MenuObject.h"
#include <engine/utility/StringHelper.h>

MENU::TextComponent::TextComponent(MenuObject& aParent, unsigned int aID)
	: MenuComponent(aParent, aID, ComponentType::Text)
{
	myString = "(None)";
	myFontName = "ProggyClean.ttf";
	mySize = FontSize_14;
	myIsCentered = true;
	myIsHidden = false;

	myColors.fill({ 1.f, 1.f, 1.f, 1.f });
	myText.SetPosition(myParent.GetPosition());
	UpdateText();
}

void MENU::TextComponent::Update(const MenuUpdateContext& aContext)
{
	aContext;
}

void MENU::TextComponent::Render()
{
	if (myIsHidden)
		return;

	myText.SetColor(myColors[(int)myParent.GetState()]);

	myText.Render();
}

void MENU::TextComponent::UpdatePosition()
{
	if (myIsCentered)
		myText.CenterTextOverPosition(myParent.GetPosition() + myPosition);
	else
		myText.SetPosition(myParent.GetPosition() + myPosition);
}

std::string MENU::TextComponent::GetText() const
{
	return myString;
}

std::string MENU::TextComponent::GetFontName() const
{
	return myFontName;
}

FontSize MENU::TextComponent::GetFontSize() const
{
	return mySize;
}

Vector2f MENU::TextComponent::GetPosition() const
{
	return myPosition;
}

Vector4f MENU::TextComponent::GetColor(ObjectState aState) const
{
	return myColors[(int)aState];
}

bool MENU::TextComponent::GetIsCentered() const
{
	return myIsCentered;
}

bool MENU::TextComponent::GetIsHidden() const
{
	return myIsHidden;
}

void MENU::TextComponent::UpdateText()
{
	std::wstring wstr = StringHelper::s2ws(myFontName);
	const wchar_t* fontFile = wstr.c_str();

	myText = Text(fontFile, mySize);

	myText.SetText(myString);
	myText.SetColor(myColors[(int)ObjectState::Default]);
	SetPosition(myPosition);
}

void MENU::TextComponent::SetFont(const std::string& aFontName)
{
	myFontName = aFontName;
	UpdateText();
}

void MENU::TextComponent::SetFontSize(FontSize aSize)
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

void MENU::TextComponent::SetColor(const Vector4f& aColor, ObjectState aState)
{
	myColors[(int)aState] = aColor;
}

void MENU::TextComponent::SetIsCentered(bool aIsCentered)
{
	myIsCentered = aIsCentered;

	UpdatePosition();
}

void MENU::TextComponent::SetIsHidden(bool aIsHidden)
{
	myIsHidden = aIsHidden;
}

void MENU::TextComponent::CenterTextOverPosition()
{
	myText.CenterTextOverPosition(myParent.GetPosition() + myPosition);
}
