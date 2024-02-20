#pragma once
#include "Component.h"

namespace ME
{
	class MenuObject;

	class TextComponent : public Component
	{
	public:
		TextComponent(MenuObject& aParent)
			: Component(aParent)
		{}

		virtual void Init() override;
		virtual void Update() override;
		virtual void Render() override;

	private:


	};
}