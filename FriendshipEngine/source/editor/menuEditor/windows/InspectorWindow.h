#pragma once
#include "Window.h"
#include <shared/postMaster/Observer.h>

namespace MENU
{
	class MenuObject;
	class SpriteComponent;
	class InspectorWindow : public WindowBase, public FE::Observer
	{
	public:
		InspectorWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);
		void Show(const UpdateContext& aContext) override;

	private:
		unsigned int mySelectedObjectID;
		unsigned int mySelectedComponentIndex;

		// Inherited via Observer
		void RecieveMessage(const FE::Message& aMessage) override;

	private:
		void AddComponentPopup(MenuObject& aObject);

		void EditSpriteComponent(const UpdateContext& aContext, MenuObject& aObject);
		void EditSpriteTextures(const UpdateContext& aContext, SpriteComponent& aSprite);
		void EditTextComponent(const UpdateContext& aContext, MenuObject& aObject);
		void EditCollider2DComponent(MenuObject& aObject);

	};
}