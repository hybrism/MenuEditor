#pragma once
#include "MenuComponent.h"
#include <engine/text/Text.h>

#include <string>

class Text;

namespace MENU
{
	class MenuObject;
	class TextComponent : public MenuComponent
	{
	public:
		TextComponent(MenuObject& aParent, unsigned int aID);

		virtual void Update(const MenuUpdateContext& aContext) override;
		virtual void Render() override;

		void OnResize(const Vector2f&) override;

		void UpdatePosition() override;

		std::string GetText() const;
		std::string GetFontName() const;
		FontSize GetFontSize() const;
		Vector2f GetPosition() const;
		Vector4f GetColor(ObjectState aState = ObjectState::Default) const;
		bool GetIsCentered() const;
		bool GetIsHidden() const;

		void SetFont(const std::string& aFontName);
		void SetFontSize(FontSize aSize);
		void SetText(const std::string& aText);
		void SetPosition(const Vector2f& aPosition);
		void SetColor(const Vector4f& aColor, ObjectState aState = ObjectState::Default);
		void SetIsCentered(bool aIsCentered);
		void SetIsHidden(bool aIsHidden);

	private:
		Text myText;
		std::string myString;
		std::string myFontName;

		std::array<Vector4f, (int)ObjectState::Count> myColors;
		
		FontSize mySize;
		bool myIsCentered;
		bool myIsHidden;

		void UpdateText();
		void CenterTextOverPosition();
	};
}
