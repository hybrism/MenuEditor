#pragma once
#include "Window.h"
#include <shared/postMaster/Observer.h>


namespace MENU
{
	class MenuObject;
	class InspectorWindow : public WindowBase, public FE::Observer
	{
	public:
		InspectorWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);
		void Show(const UpdateContext& aContext) override;

	private:
		std::string myObjectName;
		size_t mySelectedObjectID;
		size_t mySelectedComponentIndex;
		bool myIsNewObjectSelected;

		// Inherited via Observer
		void RecieveMessage(const FE::Message& aMessage) override;

	private:
		void AddComponent(MenuObject& aObject);
		void EditSpriteComponent(const UpdateContext& aContext, MenuObject& aObject);
		void EditTextComponent(const UpdateContext& aContext, MenuObject& aObject);
		void EditCollider2DComponent(MenuObject& aObject);
	};
}