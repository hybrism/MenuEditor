#pragma once
#include "Window.h"
#include <shared/postMaster/Observer.h>

class MenuObject;

namespace ME
{
	class InspectorWindow : public ME::WindowBase, public FE::Observer
	{
	public:
		InspectorWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);

		void Show(const UpdateContext& aContext) override;

	private:
		bool myIsNewObjectSelected;

		size_t mySelectedIndex;
		std::string myObjectName;

		// Inherited via Observer
		void RecieveMessage(const FE::Message& aMessage) override;

	private:
		void EditSpriteComponent(const UpdateContext& aContext, MenuObject& aObject);
		void EditTextComponent(MenuObject& aObject);
	};
}