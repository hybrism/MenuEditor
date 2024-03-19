#pragma once
#include "../math/Vector.h"
#include "FontSizeEnum.h"


class InternalTextAndFontData;
struct Font
{
	std::shared_ptr<const InternalTextAndFontData> myData;
};

class TextService;
class SpriteShader;
class Text
{
	friend class TextService;
public:
	// If this is the first time creating the text, the text will be loaded in memory, dont do this runtime
	Text(const wchar_t* aFontFile = L"arial.ttf", FontSize aFontSize = FontSize_10, unsigned char aBorderSize = 0);
	~Text();

	void Render();
	float GetWidth();
	float GetHeight();

	void CenterTextOverPosition(const Vector2f& aPosition);

	void SetText(const std::string& aText);
	void SetColor(const Vector4f& aColor);
	void SetPosition(const Vector2f& aPosition);
	void SetRotation(float aRotation);
	void SetScale(float aScale);

	std::string GetText() const { return myText; }
	Vector4f GetColor() const { return myColor;	}
	Vector2f GetPosition() const {	return myPosition;	}
	float GetScale() const { return myScale; }
	float GetRotation() const { return myRotation; }
	std::wstring GetPathAndName() const { return myFontName; }

protected:
	int FontEnumToSize(FontSize aSize);

	TextService* myTextService;
	Font myFont;
	std::wstring myFontName;
	std::string myText;
	Vector2f myPosition;
	float myScale;
	float myRotation;
	Vector4f myColor;
};


