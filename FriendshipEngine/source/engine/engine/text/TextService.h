/*
This class handles all texts that will be rendered, don't use this to show text, only use the Text class
*/
#pragma once

#include "FontFile.h"
#include "Text.h"

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
	struct FontType
	{
		short size = 0;
		unsigned char borderSize = 0;
	};

	unsigned long long FontNameToKey(const std::string& aKey) const;

	struct FT_LibraryRec_* myLibrary;

	//std::unordered_map<std::wstring, std::weak_ptr<InternalTextAndFontData>> myFontData;

	// by looking at our fonts, the only duplicate names are only up until 7 characters in
	// meaning we can use those instead of the full name
	// additionally, we can convert the chars straight up to an ull and use that as a key since
	// the unordered map hashing is consonsistent with strings
	std::unordered_map<
		unsigned long long,
		std::unordered_map<
			int,
			std::shared_ptr<InternalTextAndFontData>
		>
	> myFontData;
	
};