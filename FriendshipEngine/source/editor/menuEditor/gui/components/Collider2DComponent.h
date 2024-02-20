#pragma once
#include "Component.h"

namespace ME
{
	class MenuObject;

	class Collider2DComponent : public Component
	{
	public:
		Collider2DComponent(MenuObject& aParent)
			: Component(aParent)
		{}

		virtual void Init() override;
		virtual void Update() override;
		virtual void Render() override;

	private:


	};
}