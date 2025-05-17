#pragma once
#include "Window.h"
#include <engine/math/Vector.h>

namespace MENU
{
	class MenuObjectHierarchy : public WindowBase
	{
	public:
		MenuObjectHierarchy(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);

		void Show(const MenuEditorUpdateContext& aContext) override;

	private:
		unsigned int mySelectedStateID;
		unsigned int mySelectedObjectID;
		unsigned int myRightClickedObjectID;
		unsigned int myRightClickedStateID;

		Vector2f myViewportCenter;
		std::string myNewStateName;

		void PushMenuObjectToInspector(unsigned int aID);
		void AddStateButton(const MenuEditorUpdateContext& aContext);
		void AddObjectButton(const MenuEditorUpdateContext& aContext);

		void DuplicateState(const MenuEditorUpdateContext& aContext, unsigned int aStateIdToCopy);
		void DuplicateObject(const MenuEditorUpdateContext& aContext, unsigned int aObjectIDToCopy);
	};
}