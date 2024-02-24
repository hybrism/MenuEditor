#pragma once
#include "Window.h"
#include <engine/math/Vector.h>

namespace MENU
{
	class MenuObjectHierarchy : public MENU::WindowBase
	{
	public:
		MenuObjectHierarchy(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);

		void Show(const UpdateContext& aContext) override;

	private:
		size_t mySelectedIndex;
		Vector2f myViewportCenter;

		void PushMenuObjectToInspector();
	};
}