#include "pch.h"
#include "../text/Text.h"

#include "../Paths.h"
#include "../utility/Error.h"
#include "../text/TextService.h"
#include "../graphics/GraphicsEngine.h"
#include "../graphics/renderer/SpriteRenderer.h"

Text::Text(const wchar_t* aFontFile, FontSize aFontSize, unsigned char aBorderSize)
	: myTextService(GraphicsEngine::GetInstance()->GetSpriteRenderer().GetTextService())
{
	myColor = Vector4f(1.f, 1.f, 1.f, 1.f);
	myScale = 1.0f;
	myFontName = aFontFile;
	myFont = myTextService->GetOrLoad(myFontName, aBorderSize, FontEnumToSize(aFontSize));
	myRotation = 0.0f;
}

Text::~Text() {}

void Text::Render()
{
	if (!myTextService)
		return;

	GraphicsEngine::GetInstance()->GetSpriteRenderer().DrawText(this);
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

int Text::FontEnumToSize(FontSize aSize)
{
	switch (aSize)
	{
	case FontSize_6:
		return 6;
	case FontSize_8:
		return 8; 
	case FontSize_9:
		return 9; 
	case FontSize_10:
		return 10;
	case FontSize_11:
		return 11; 
	case FontSize_12:
		return 12; 
	case FontSize_14:
		return 14; 
	case FontSize_18:
		return 15; 
	case FontSize_24:
		return 24; 
	case FontSize_30:
		return 30; 
	case FontSize_36:
		return 36; 
	case FontSize_48:
		return 48; 
	case FontSize_60:
		return 60; 
	case FontSize_72:
		return 72;
	case FontSize_Count:
	default:
		break;
	}

	return 0;
}
