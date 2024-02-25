#pragma once
#include "MenuComponent.h"
#include <engine/math/Vector.h>

#include <string>

class Text;

namespace MENU
{
	class MenuObject;
	class TextComponent : public MenuComponent
	{
	public:
		TextComponent(MenuObject& aParent);

		virtual void Update() override;
		virtual void Render() override;

		std::string GetText();
		const Vector2f GetPosition();
		const Vector4f GetColor();

		void SetText(const std::string& aText);
		void SetPosition(const Vector2f& aPosition);
		void SetColor(const Vector4f& aColor);

	private:
		Text* myText;
		std::string myString;
	};
}
