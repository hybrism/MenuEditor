/*
This class handles all texts that will be rendered, don't use this to show text, only use the Text class
*/
#pragma once

#include "fontfile.h"
#include "text.h"

class Text;
class TextService
{
public:
	TextService();
	~TextService();

	void Init();

	Font GetOrLoad(std::wstring aFontFile, unsigned char aBorderSize, int aSize);
	bool Draw(Text& aText, SpriteShader* aCustomShaderToRenderWith = nullptr);
	float GetSentenceWidth(Text& aText);
	float GetSentenceHeight(Text& aText);

private:
	struct FT_LibraryRec_* myLibrary;

	std::unordered_map<std::wstring, std::weak_ptr<InternalTextAndFontData>> myFontData;
};