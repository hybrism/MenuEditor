#pragma once
#include "MenuComponent.h"
#include <engine/text/text.h>

#include <string>

class Text;

namespace MENU
{
	enum class eSize
	{
		Small,
		Medium,
		Large
	};

	class MenuObject;
	class TextComponent : public MenuComponent
	{

	public:
		TextComponent(MenuObject& aParent);

		virtual void Update() override;
		virtual void Render() override;

		void UpdatePosition() override;

		std::string GetText();
		std::string GetFontName();
		eSize GetFontSize();
		const Vector2f GetPosition();
		const Vector4f GetColor();
		bool GetIsCentered() const;

		void SetFont(const std::string& aFontName);
		void SetFontSize(eSize aSize);
		void SetText(const std::string& aText);
		void SetPosition(const Vector2f& aPosition);
		void SetColor(const Vector4f& aColor);
		void SetIsCentered(bool aBool);


	private:
		Text myText;
		std::string myString;
		std::string myFontName;
		eSize mySize;
		bool myIsCentered;

		void UpdateText();
		FontSize SizeAsFontSize();
		void CenterTextOverPosition();

	};
}
