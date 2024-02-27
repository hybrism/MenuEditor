#include "pch.h"
#include "../text/Text.h"

#include "../Paths.h"
#include "../utility/Error.h"
#include "../text/TextService.h"
#include "../graphics/GraphicsEngine.h"

Text::Text(const wchar_t* aFontFile, FontSize aFontSize, unsigned char aBorderSize)
	: myTextService(&GraphicsEngine::GetInstance()->GetTextService())
{
	myColor = Vector4f(1.f, 1.f, 1.f, 1.f);
	myScale = 1.0f;
	myFontName = aFontFile;
	myFont = myTextService->GetOrLoad(myFontName, aBorderSize, aFontSize);
	myRotation = 0.0f;
}

Text::~Text() {}

void Text::Render()
{
	if (!myTextService)
	{
		return;
	}
	if (!myTextService->Draw(*this))
	{
		PrintE("[Text.cpp] Text rendering error! Trying to render a text where the resource has been deleted! Did you clear the memory for this font? OR: Did you set the correct working directory?");
	}
}

float Text::GetWidth()
{
	if (!myTextService)
	{
		return 0.0f;
	}

	return myTextService->GetSentenceWidth(*this);
}

float Text::GetHeight()
{
	if (!myTextService)
	{
		return 0.0f;
	}

	return myTextService->GetSentenceHeight(*this);
}

void Text::CenterTextOverPosition(const Vector2f& aPosition)
{
	float width = myTextService->GetSentenceWidth(*this);
	float height = myTextService->GetSentenceHeight(*this);

	myPosition.x = aPosition.x - (width * 0.5f);
	myPosition.y = aPosition.y - (height * 0.25f);
}

void Text::SetColor(const Vector4f& aColor)
{
	myColor = aColor;
}

void Text::SetText(const std::string& aText)
{
	myText = aText;
}

void Text::SetPosition(const Vector2f& aPosition)
{
	myPosition = aPosition;
}

void Text::SetRotation(float aRotation)
{
	myRotation = aRotation;
}

void Text::SetScale(float aScale)
{
	myScale = aScale;
}
