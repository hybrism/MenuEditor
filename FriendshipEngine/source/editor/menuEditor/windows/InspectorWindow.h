#pragma once
#include "Window.h"
#include <engine/math/Vector.h>
#include <shared/postMaster/Observer.h>

namespace MENU
{
	class MenuObject;
	class SpriteComponent;
	class InspectorWindow : public WindowBase, public FE::Observer
	{
	public:
		InspectorWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);
		void Show(const MenuEditorUpdateContext& aContext) override;

	private:
		Vector2f myViewportSize;
		unsigned int mySelectedObjectID;
		unsigned int mySelectedComponentIndex;

		// Inherited via Observer
		void RecieveMessage(const FE::Message& aMessage) override;

	private:
		void AddComponentButton(MenuObject& aObject);

		void EditSpriteComponent(const MenuEditorUpdateContext& aContext, MenuObject& aObject);
		void EditSpriteTextures(const MenuEditorUpdateContext& aContext, SpriteComponent& aSprite);
		void EditTextComponent(const MenuEditorUpdateContext& aContext, MenuObject& aObject);
		void EditCollider2DComponent(const MenuEditorUpdateContext& aContext, MenuObject& aObject);
		void EditCommandComponent(const MenuEditorUpdateContext& aContext, MenuObject& aObject);

	};
}