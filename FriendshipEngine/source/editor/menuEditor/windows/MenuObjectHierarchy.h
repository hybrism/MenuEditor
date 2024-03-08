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
		unsigned int mySelectedStateID;
		unsigned int mySelectedObjectID;
		unsigned int myRightClickedObjectID;
		Vector2f myViewportCenter;
		std::string myNewStateName;

		void PushMenuObjectToInspector(unsigned int aID);
		void AddStateButton(const UpdateContext& aContext);
		void AddObjectButton(const UpdateContext& aContext);
	};
}