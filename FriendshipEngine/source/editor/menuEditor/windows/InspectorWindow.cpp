#include "InspectorWindow.h"
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <engine/text/text.h>
#include <engine/utility/StringHelper.h>

#include "../gui/MenuObject.h"
#include "../gui/MenuHandler.h"
#include "../gui/ObjectManager.h"
#include "../gui/components/SpriteComponent.h"
#include "../gui/components/TextComponent.h"
#include "../gui/components/Collider2DComponent.h"
#include "../gui/components/CommandComponent.h"

#include <shared/postMaster/PostMaster.h>

MENU::InspectorWindow::InspectorWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{
	mySelectedObjectID = UINT_MAX;
	mySelectedComponentIndex = (size_t)ComponentType::Count;

	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::PushEntityToInspector);
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::NewMenuLoaded);
}

void MENU::InspectorWindow::Show(const UpdateContext& aContext)
{
	if (!myData.isOpen)
		return;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		if (mySelectedObjectID == UINT_MAX) //Early out
		{
			ImGui::End();
			return;
		}

		MenuObject& selectedObject = aContext.menuHandler->GetObjectFromID(mySelectedObjectID);
		ImGui::PushID(selectedObject.GetID());

		{
			ImGui::InputText("##", &selectedObject.GetName());
			Vector2f position = selectedObject.GetPosition();
			if (ImGui::DragFloat2("Position", &position.x))
				selectedObject.SetPosition(position);
		}

		AddComponentButton(selectedObject);

		if (selectedObject.HasComponent<SpriteComponent>())
			EditSpriteComponent(aContext, selectedObject);

		if (selectedObject.HasComponent<TextComponent>())
			EditTextComponent(aContext, selectedObject);

		if (selectedObject.HasComponent<Collider2DComponent>())
			EditCollider2DComponent(selectedObject);

		if (selectedObject.HasComponent<CommandComponent>())
			EditCommandComponent(selectedObject);

		ImGui::PopID();
	}
	ImGui::End();
}

void MENU::InspectorWindow::RecieveMessage(const FE::Message& aMessage)
{
	switch (aMessage.myEventType)
	{
	case FE::eMessageType::PushEntityToInspector:
	{
		mySelectedObjectID = std::any_cast<unsigned int>(aMessage.myMessage);
		break;
	}
	case FE::eMessageType::NewMenuLoaded:
	{
		mySelectedObjectID = UINT_MAX;
		break;
	}
	default:
		break;
	}
}

void MENU::InspectorWindow::AddComponentButton(MenuObject& aObject)
{
	ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 24));
	if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonLeft))
	{
		ImGui::Text("Chose component: ");

		const char* comboPreviewValue = ComponentNames[mySelectedComponentIndex];  // Pass in the preview value visible before opening the combo (it could be anything)
		if (ImGui::BeginCombo("Select Component", comboPreviewValue))
		{
			for (unsigned int i = 0; i < IM_ARRAYSIZE(ComponentNames); i++)
			{
				const bool isSelected = (mySelectedComponentIndex == i);
				if (ImGui::Selectable(ComponentNames[i], isSelected))
				{
					mySelectedComponentIndex = i;
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Add", ImVec2(120, 0)))
		{
			if ((ComponentType)mySelectedComponentIndex == ComponentType::Count)
			{
				mySelectedComponentIndex = (size_t)ComponentType::Count;
			}
			else
			{
				aObject.AddComponentOfType((ComponentType)mySelectedComponentIndex);
				mySelectedComponentIndex = (size_t)ComponentType::Count;
			}
			ImGui::CloseCurrentPopup();

		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			mySelectedComponentIndex = (size_t)ComponentType::Count;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

}

void MENU::InspectorWindow::EditSpriteComponent(const UpdateContext& aContext, MenuObject& aObject)
{
	auto sprites = aObject.GetComponents<SpriteComponent>();

	ImGui::PushID("Sprite");
	for (int componentIndex = 0; componentIndex < sprites.size(); componentIndex++)
	{
		SpriteComponent& sprite = static_cast<SpriteComponent&>(*sprites[componentIndex]);

		Vector2f position = sprite.GetPosition();
		ImGui::PushID(componentIndex);
		ImGui::SeparatorText("Sprite");
		ImGui::Text("ComponentID: %i", sprite.GetID());

		Vector2f texSize = sprite.GetTextureSize();
		ImGui::Text("Texture size x: %i y: %i", (int)texSize.x, (int)texSize.y);

		EditSpriteTextures(aContext, sprite);

		if (ImGui::TreeNode("More"))
		{
			if (ImGui::DragFloat2("Position", &position.x))
				sprite.SetPosition(position);

			ImGui::DragFloat2("Size", &sprite.GetSize().x, 0.01f);
			ImGui::DragFloat2("Pivot", &sprite.GetPivot().x, 0.001f, 0.f, 1.f);
			ImGui::DragFloat2("ScaleMultiplier", &sprite.GetScaleMultiplier().x, 0.01f);

			//ImGui::DragFloat("Rotation", &sprite.GetRotation(), 0.01f);
			ClipValue& clip = sprite.GetClipValue();
			ImGui::SetNextItemWidth(50.f);
			ImGui::DragFloat("Left", &clip.left, 0.01f, 0.f, 1.f);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.f);
			ImGui::DragFloat("Right", &clip.right, 0.01f, 0.f, 1.f);
			ImGui::SetNextItemWidth(50.f);
			ImGui::DragFloat("Upper", &clip.upper, 0.01f, 0.f, 1.f);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.f);
			ImGui::DragFloat("Down", &clip.down, 0.01f, 0.f, 1.f);
			ImGui::Checkbox("Is Hidden", &sprite.GetIsHidden());

			if (ImGui::Button("RemoveComponent", ImVec2(ImGui::GetContentRegionAvail().x, 24)))
				aObject.RemoveComponent(sprite.GetID());

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	ImGui::PopID();
}

void MENU::InspectorWindow::EditSpriteTextures(const UpdateContext& aContext, SpriteComponent& aSprite)
{
	const char* textureState[] = { "Default", "Hovered", "Pressed" };
	Texture* currentItem = aSprite.GetTexture();
	for (size_t textureStateIndex = 0; textureStateIndex < (int)TextureState::Count; textureStateIndex++)
	{
		if (ImGui::BeginCombo(textureState[textureStateIndex], aSprite.GetTexturePath((TextureState)textureStateIndex).c_str()))
		{
			for (size_t i = 0; i < aContext.assets.textures.size(); i++)
			{
				bool isSelected = (currentItem == aContext.assets.textures[i]);

				if (ImGui::Selectable(aContext.assets.textureIdToName[i].c_str(), isSelected))
					aSprite.SetTexture(aContext.assets.textures[i], aContext.assets.textureIdToName[i], (TextureState)textureStateIndex);

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::ColorEdit4(textureState[textureStateIndex], &aSprite.GetColor((TextureState)textureStateIndex).x);
	}
}

void MENU::InspectorWindow::EditTextComponent(const UpdateContext& aContext, MenuObject& aObject)
{
	auto texts = aObject.GetComponents<TextComponent>();

	ImGui::PushID("Text");
	for (int componentIndex = 0; componentIndex < texts.size(); componentIndex++)
	{
		TextComponent& text = static_cast<TextComponent&>(*texts[componentIndex]);

		ImGui::PushID(componentIndex);
		ImGui::SeparatorText("Text");
		ImGui::Text("ComponentID: %i", text.GetID());

		std::string string = text.GetText();
		Vector2f position = text.GetPosition();
		Vector4f color = text.GetColor();

		if (ImGui::InputText("Text", &string))
			text.SetText(string);

		std::string currentFont = text.GetFontName();
		if (ImGui::BeginCombo("Select Font", text.GetFontName().c_str()))
		{
			for (size_t i = 0; i < aContext.assets.fontFiles.size(); i++)
			{
				bool isSelected = (currentFont == aContext.assets.fontFiles[i]);

				if (ImGui::Selectable(aContext.assets.fontFiles[i].c_str(), isSelected))
				{
					text.SetFont(aContext.assets.fontFiles[i]);
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		const char* sizeDisplayNames[] = { "Small", "Medium", "Large" };
		const char* comboPreviewValue = sizeDisplayNames[(int)text.GetFontSize()];  // Pass in the preview value visible before opening the combo (it could be anything)

		if (ImGui::BeginCombo("Select Font Size", comboPreviewValue))
		{
			for (int i = 0; i < IM_ARRAYSIZE(sizeDisplayNames); i++)
			{
				const bool isSelected = ((int)text.GetFontSize() == i);
				if (ImGui::Selectable(sizeDisplayNames[i], isSelected))
				{
					text.SetFontSize((eSize)i);
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		bool isCentered = text.GetIsCentered();
		if (ImGui::Checkbox("Center text over Position", &isCentered))
			text.SetIsCentered(isCentered);

		if (ImGui::TreeNode("More"))
		{

			if (ImGui::DragFloat2("Position", &position.x))
				text.SetPosition(position);

			if (ImGui::ColorEdit4("Color", &color.x))
				text.SetColor(color);

			if (ImGui::Button("RemoveComponent", ImVec2(ImGui::GetContentRegionAvail().x, 24)))
				aObject.RemoveComponent(text.GetID());

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	ImGui::PopID();
}

void MENU::InspectorWindow::EditCollider2DComponent(MenuObject& aObject)
{
	Collider2DComponent& collider = aObject.GetComponent<Collider2DComponent>();
	ImGui::PushID("2DCollider");
	ImGui::SeparatorText("Collider");
	ImGui::Text("ComponentID: %i", collider.GetID());

	Vector2f position = collider.GetPosition();
	Vector2f size = collider.GetSize();

	if (ImGui::DragFloat2("Size", &size.x))
		collider.SetSize(size);
	if (ImGui::TreeNode("More"))
	{
		if (ImGui::DragFloat2("Position", &position.x))
			collider.SetPosition(position);

		if (ImGui::Button("RemoveComponent", ImVec2(ImGui::GetContentRegionAvail().x, 24)))
			aObject.RemoveComponent(collider.GetID());

		ImGui::TreePop();
	}

	ImGui::Spacing();
	ImGui::PopID();
}

void MENU::InspectorWindow::EditCommandComponent(MenuObject& aObject)
{
	CommandComponent& command = aObject.GetComponent<CommandComponent>();
	ImGui::PushID("Command");
	ImGui::SeparatorText("Command");
	ImGui::Text("ComponentID: %i", command.GetID());

	eCommandType currentType = command.GetCommandType();
	if (ImGui::BeginCombo("Commands", CommandNames[(int)currentType]))
	{
		for (size_t i = 0; i < IM_ARRAYSIZE(CommandNames); i++)
		{
			bool isSelected = ((int)currentType == i);

			if (ImGui::Selectable(CommandNames[i], isSelected))
				command.SetCommandType((eCommandType)i);

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	switch (command.GetCommandType())
	{
	case eCommandType::PopMenu:
	{
		break;
	}
	case eCommandType::PushMenu:
	{
		static int menuID = 0;
		ImGui::InputInt("Menu to load", &menuID);
		command.SetCommandData({ (ID)menuID });
		break;
	}
	case eCommandType::LoadLevel:
	{
		static std::string level = "(None)";
		ImGui::InputText("Level to load", &level);
		command.SetCommandData({ level });
		break;
	}
	case eCommandType::ResumeGame:
	{
		break;
	}
	case eCommandType::BackToMainMenu:
	{
		break;
	}
	case eCommandType::QuitGame:
	{
		break;
	}
	default:
		break;
	}


	ImGui::Spacing();
	ImGui::PopID();
}
