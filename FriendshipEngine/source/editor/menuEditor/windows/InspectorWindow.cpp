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

#include <shared/postMaster/PostMaster.h>

MENU::InspectorWindow::InspectorWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{
	mySelectedObjectID = UINT_MAX;
	mySelectedComponentIndex = (size_t)ComponentType::Count;
	myIsNewObjectSelected = false;

	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::PushEntityToInspector);
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::NewMenuLoaded);
}

void MENU::InspectorWindow::Show(const UpdateContext& aContext)
{
	if (!myData.isOpen)
		return;

	if (myIsNewObjectSelected)
	{
		//Get values from object
		myObjectName = aContext.menuHandler->GetObjectFromID(mySelectedObjectID).GetName();

		myIsNewObjectSelected = false;
	}

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
			ImGui::InputText("##", &myObjectName);
			ImGui::SameLine();
			if (ImGui::Button("Save Name"))
				selectedObject.SetName(myObjectName);

			Vector2f position = selectedObject.GetPosition();
			if (ImGui::DragFloat2("Position", &position.x))
				selectedObject.SetPosition(position);
		}

		if (selectedObject.HasComponent<SpriteComponent>())
			EditSpriteComponent(aContext, selectedObject);

		if (selectedObject.HasComponent<TextComponent>())
			EditTextComponent(aContext, selectedObject);

		if (selectedObject.HasComponent<Collider2DComponent>())
			EditCollider2DComponent(selectedObject);

		if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 24)))
		{
			ImGui::OpenPopup("Add Component");
		}

		AddComponent(selectedObject);

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
		mySelectedObjectID = std::any_cast<size_t>(aMessage.myMessage);
		myIsNewObjectSelected = true;
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

void MENU::InspectorWindow::AddComponent(MenuObject& aObject)
{
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Add Component", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Chose component: ");

		//TODO: Make this more dynamic
		const char* componentNames[] = { "SpriteComponent", "ColliderComponent", "TextComponent", "None" };
		const char* comboPreviewValue = componentNames[mySelectedComponentIndex];  // Pass in the preview value visible before opening the combo (it could be anything)

		if (ImGui::BeginCombo("Select Component", comboPreviewValue))
		{
			for (int i = 0; i < IM_ARRAYSIZE(componentNames); i++)
			{
				const bool isSelected = (mySelectedComponentIndex == i);
				if (ImGui::Selectable(componentNames[i], isSelected))
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
				ImGui::CloseCurrentPopup();

			aObject.AddComponentOfType((ComponentType)mySelectedComponentIndex);
			mySelectedComponentIndex = (size_t)ComponentType::Count;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			mySelectedComponentIndex = 0;
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

		Vector2f texSize = sprite.GetTextureSize();
		ImGui::Text("Texture size x: %i y: %i", (int)texSize.x, (int)texSize.y);
		Texture* currentItem = sprite.GetTexture();
		if (ImGui::BeginCombo("Select Texture", sprite.GetTexturePath().c_str()))
		{
			for (size_t i = 0; i < aContext.assets.textures.size(); i++)
			{
				bool isSelected = (currentItem == aContext.assets.textures[i]);

				if (ImGui::Selectable(aContext.assets.textureIdToName[i].c_str(), isSelected))
					sprite.SetTexture(aContext.assets.textures[i], aContext.assets.textureIdToName[i]);

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::Spacing();

		if (ImGui::DragFloat2("Position", &position.x))
			sprite.SetPosition(position);

		ImGui::DragFloat2("Size", &sprite.GetSize().x, 0.01f);
		ImGui::DragFloat2("Pivot", &sprite.GetPivot().x, 0.001f, 0.f, 1.f);
		ImGui::DragFloat2("ScaleMultiplier", &sprite.GetScaleMultiplier().x, 0.01f);
		ImGui::ColorEdit4("Color", &sprite.GetColor().x);

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

		ImGui::PopID();
	}

	ImGui::PopID();
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

		if (ImGui::DragFloat2("Position", &position.x))
			text.SetPosition(position);

		bool isCentered = text.GetIsCentered();
		if (ImGui::Checkbox("Center text over Position", &isCentered))
			text.SetIsCentered(isCentered);

		if (ImGui::ColorEdit4("Color", &color.x))
			text.SetColor(color);

		ImGui::PopID();
	}

	ImGui::PopID();
}

void MENU::InspectorWindow::EditCollider2DComponent(MenuObject& aObject)
{
	Collider2DComponent& collider = aObject.GetComponent<Collider2DComponent>();
	ImGui::PushID("2DCollider");
	ImGui::SeparatorText("Collider");

	Vector2f position = collider.GetPosition();
	Vector2f size = collider.GetSize();

	if (ImGui::DragFloat2("Position", &position.x))
		collider.SetPosition(position);

	if (ImGui::DragFloat2("Size", &size.x))
		collider.SetSize(size);

	ImGui::Spacing();
	ImGui::PopID();
}
