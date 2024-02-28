#pragma once
#include "Window.h"
#include <engine/math/Vector.h>

namespace MENU
{
	class MenuObjectHierarchy : public WindowBase
	{
	public:
		MenuObjectHierarchy(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);

		void Show(const UpdateContext& aContext) override;

	private:
		unsigned int mySelectedID;
		unsigned int myRightClickedID;
		Vector2f myViewportCenter;

		void PushMenuObjectToInspector(unsigned int aID);
	};
}