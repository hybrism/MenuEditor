#pragma once
#include "../math/Vector.h"
#include <memory>

enum FontSize
{
	FontSize_6 = 6,
	FontSize_8 = 8,
	FontSize_9 = 9,
	FontSize_10 = 10,
	FontSize_11 = 11,
	FontSize_12 = 12,
	FontSize_14 = 14,
	FontSize_18 = 18,
	FontSize_24 = 24,
	FontSize_30 = 30,
	FontSize_36 = 36,
	FontSize_48 = 48,
	FontSize_60 = 60,
	FontSize_72 = 72,
	FontSize_Count
};

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
	TextService* myTextService;
	Font myFont;
	std::wstring myFontName;
	std::string myText;
	Vector2f myPosition;
	float myScale;
	float myRotation;
	Vector4f myColor;
};


