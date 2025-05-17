#include "InspectorWindow.h"
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <engine/text/text.h>
#include <engine/utility/StringHelper.h>
#include <engine/graphics/GraphicsEngine.h>

#include <game/gui/MenuObject.h>
#include <game/gui/MenuHandler.h>
#include <game/gui/ObjectManager.h>
#include <game/gui/components/TextComponent.h>
#include <game/gui/components/SpriteComponent.h>
#include <game/gui/components/CommandComponent.h>
#include <game/gui/components/Collider2DComponent.h>
#include <game/gui/components/InteractableComponent.h>

#include <shared/postMaster/PostMaster.h>

MENU::InspectorWindow::InspectorWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{
	mySelectedObjectID = UINT_MAX;
	mySelectedComponentIndex = (size_t)ComponentType::Count;

	Vector2i center = GraphicsEngine::GetInstance()->DX().GetViewportDimensions();
	myViewportSize = { (float)center.x, (float)center.y };

	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::PushEntityToInspector);
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::NewMenuLoaded);
}

void MENU::InspectorWindow::Show(const MenuEditorUpdateContext& aContext)
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
			ImGui::InputText("##", &selectedObject.GetName(), ImGuiInputTextFlags_AutoSelectAll);
			Vector2f pixelPosition = selectedObject.GetPosition();
			Vector2f screenPosition = { pixelPosition.x / myViewportSize.x, pixelPosition.y / myViewportSize.y };

			if (ImGui::DragFloat2("Position", &screenPosition.x, 0.001f))
				selectedObject.SetPosition({ screenPosition.x * myViewportSize.x, screenPosition.y * myViewportSize.y });

			if (ImGui::DragFloat2("Position (Pixel)", &pixelPosition.x))
				selectedObject.SetPosition(pixelPosition);
		}

		AddComponentButton(selectedObject);

		if (selectedObject.HasComponent<SpriteComponent>())
			EditSpriteComponent(aContext, selectedObject);

		if (selectedObject.HasComponent<TextComponent>())
			EditTextComponent(aContext, selectedObject);

		if (selectedObject.HasComponent<Collider2DComponent>())
			EditCollider2DComponent(aContext, selectedObject);

		if (selectedObject.HasComponent<CommandComponent>())
			EditCommandComponent(aContext, selectedObject);

		if (selectedObject.HasComponent<InteractableComponent>())
			EditInteractableComponent(aContext, selectedObject);

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
		ImGui::Text("Choose component: ");

		const char* comboPreviewValue = ComponentNames[mySelectedComponentIndex];
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

void MENU::InspectorWindow::EditSpriteComponent(const MenuEditorUpdateContext& aContext, MenuObject& aObject)
{
	auto sprites = aObject.GetComponents<SpriteComponent>();

	ImGui::PushID("Sprite");
	for (int componentIndex = 0; componentIndex < sprites.size(); componentIndex++)
	{
		SpriteComponent& sprite = static_cast<SpriteComponent&>(*sprites[componentIndex]);

		Vector2f position = sprite.GetPosition();
		ImGui::PushID(componentIndex);
		ImGui::SeparatorText("Sprite");
		ImGui::SameLine();
		ImGui::InputText("Name", &sprite.myName, ImGuiInputTextFlags_AutoSelectAll);

		if (aContext.showDebugData)
		{
			ImGui::Text("ComponentID: %i", sprite.GetID());
			ImGui::Text("ParentID: %i", sprite.GetParent().GetID());
			Vector2f texSize = sprite.GetTextureSize();
			ImGui::Text("Texture size x: %i y: %i", (int)texSize.x, (int)texSize.y);
		}

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

void MENU::InspectorWindow::EditSpriteTextures(const MenuEditorUpdateContext& aContext, SpriteComponent& aSprite)
{
	std::shared_ptr<Texture> currentItem = aSprite.GetTexture();
	for (size_t textureStateIndex = 0; textureStateIndex < (int)ObjectState::Count; textureStateIndex++)
	{
		if (ImGui::BeginCombo(ObjectStates[textureStateIndex], aSprite.GetTexturePath((ObjectState)textureStateIndex).c_str()))
		{
			for (size_t i = 0; i < aContext.assets.textures.size(); i++)
			{
				bool isSelected = (currentItem == aContext.assets.textures[i]);

				if (ImGui::Selectable(aContext.assets.textureIdToName[i].c_str(), isSelected))
					aSprite.SetTexture(aContext.assets.textures[i], aContext.assets.textureIdToName[i], (ObjectState)textureStateIndex);

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::ColorEdit4(ObjectStates[textureStateIndex], &aSprite.GetColor((ObjectState)textureStateIndex).x);
	}
}

void MENU::InspectorWindow::EditTextComponent(const MenuEditorUpdateContext& aContext, MenuObject& aObject)
{
	auto texts = aObject.GetComponents<TextComponent>();

	ImGui::PushID("Text");
	for (int componentIndex = 0; componentIndex < texts.size(); componentIndex++)
	{
		TextComponent& text = static_cast<TextComponent&>(*texts[componentIndex]);

		ImGui::PushID(componentIndex);
		ImGui::SeparatorText("Text");
		ImGui::SameLine();
		ImGui::InputText("Name", &text.myName, ImGuiInputTextFlags_AutoSelectAll);

		if (aContext.showDebugData)
		{
			ImGui::Text("ComponentID: %i", text.GetID());
			ImGui::Text("ParentID: %i", text.GetParent().GetID());
		}

		std::string string = text.GetText();
		Vector2f position = text.GetPosition();

		if (ImGui::InputText("Text", &string, ImGuiInputTextFlags_AutoSelectAll))
			text.SetText(string);

		std::string currentFont = text.GetFontName();
		if (ImGui::BeginCombo("Font", text.GetFontName().c_str()))
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


		const char* comboPreviewValue = FontSizeDisplayNames[(int)text.GetFontSize()];

		if (ImGui::BeginCombo("Font Size", comboPreviewValue))
		{
			for (int i = 0; i < IM_ARRAYSIZE(FontSizeDisplayNames); i++)
			{
				const bool isSelected = ((int)text.GetFontSize() == i);
				if (ImGui::Selectable(FontSizeDisplayNames[i], isSelected))
				{
					text.SetFontSize((FontSize)i);
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

			for (size_t i = 0; i < (int)ObjectState::Count; i++)
			{
				Vector4f color = text.GetColor((ObjectState)i);
				if (ImGui::ColorEdit4(ObjectStates[i], &color.x))
					text.SetColor(color, (ObjectState)i);
			}


			if (ImGui::Button("RemoveComponent", ImVec2(ImGui::GetContentRegionAvail().x, 24)))
				aObject.RemoveComponent(text.GetID());

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	ImGui::PopID();
}

void MENU::InspectorWindow::EditCollider2DComponent(const MenuEditorUpdateContext& aContext, MenuObject& aObject)
{
	Collider2DComponent& collider = aObject.GetComponent<Collider2DComponent>();
	ImGui::PushID("2DCollider");
	ImGui::SeparatorText("Collider");

	if (aContext.showDebugData)
	{
		ImGui::Text("ComponentID: %i", collider.GetID());
		ImGui::Text("ParentID: %i", collider.GetParent().GetID());
	}

	Vector2f position = collider.GetPosition();
	Vector2f size = collider.GetSize();

	if (ImGui::DragFloat2("Size", &size.x))
		collider.SetSize(size);

	ImGui::SameLine();
	if (aObject.HasComponent<SpriteComponent>())
	{
		if (ImGui::Button("Sprite"))
		{
			SpriteComponent& sprite = aObject.GetComponent<SpriteComponent>();
			collider.SetSize(sprite.GetSize());
		}
	}

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

void MENU::InspectorWindow::EditCommandComponent(const MenuEditorUpdateContext& aContext, MenuObject& aObject)
{
	CommandComponent& command = aObject.GetComponent<CommandComponent>();
	ImGui::PushID("Command");
	ImGui::SeparatorText("Command");

	if (aContext.showDebugData)
	{
		ImGui::Text("ComponentID: %i", command.GetID());
		ImGui::Text("ParentID: %i", command.GetParent().GetID());
	}

	static std::string commandInputString = "(None)";
	static float commandInputFloat = 0.f;
	static int commandInputInt = 0;

	eCommandType currentType = command.GetCommandType();
	if (ImGui::BeginCombo("Commands", CommandNames[(int)currentType]))
	{
		for (size_t i = 0; i < IM_ARRAYSIZE(CommandNames); i++)
		{
			bool isSelected = ((int)currentType == i);

			if (ImGui::Selectable(CommandNames[i], isSelected))
			{
				command.SetCommandType((eCommandType)i);
				commandInputString = "(None)";
				commandInputFloat = 0.f;
			}

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

		if (ImGui::BeginCombo("Select Menu", commandInputString.c_str()))
		{
			auto& states = aContext.menuHandler->GetAllStates();
			for (size_t i = 0; i < states.size(); i++)
			{
				bool isSelected = ((ID)menuID == states[i].id);

				if (ImGui::Selectable(states[i].name.c_str(), isSelected))
				{
					menuID = states[i].id;
					commandInputString = states[i].name;
					command.SetCommandData({ (ID)menuID });
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		break;
	}
	case eCommandType::LoadLevel:
	{
		if (std::holds_alternative<std::string>(command.GetCommandData().data))
			commandInputString = std::get<std::string>(command.GetCommandData().data);

		ImGui::InputText("Level to load", &commandInputString, ImGuiInputTextFlags_AutoSelectAll);
		command.SetCommandData({ commandInputString });
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
	case eCommandType::Resolution:
	{
		if (std::holds_alternative<int>(command.GetCommandData().data))
			commandInputInt = std::get<int>(command.GetCommandData().data);

		ImGui::DragInt("Incrrease -/+", &commandInputInt, 1, -1, 1);
		command.SetCommandData({ commandInputInt });

		break;
	}
	case eCommandType::SfxVolume:
	{
		if (std::holds_alternative<float>(command.GetCommandData().data))
			commandInputFloat = std::get<float>(command.GetCommandData().data);

		ImGui::DragFloat("Volume -/+", &commandInputFloat, 0.01f, -1.f, 1.f, "%.2f");
		command.SetCommandData({ commandInputFloat });
		break;
	}
	case eCommandType::MusicVolume:
	{
		if (std::holds_alternative<float>(command.GetCommandData().data))
			commandInputFloat = std::get<float>(command.GetCommandData().data);

		ImGui::DragFloat("Volume -/+", &commandInputFloat, 0.01f, -1.f, 1.f, "%.2f");
		command.SetCommandData({ commandInputFloat });
		break;
	}
	default:
		break;
	}

	if (ImGui::TreeNode("More"))
	{
		if (ImGui::Button("RemoveComponent", ImVec2(ImGui::GetContentRegionAvail().x, 24)))
			aObject.RemoveComponent(command.GetID());

		ImGui::TreePop();
	}

	ImGui::Spacing();
	ImGui::PopID();
}

void MENU::InspectorWindow::EditInteractableComponent(const MenuEditorUpdateContext& aContext, MenuObject& aObject)
{
	InteractableComponent& interactable = aObject.GetComponent<InteractableComponent>();
	ImGui::PushID("Interactable");
	ImGui::SeparatorText("Interactable");

	if (aContext.showDebugData)
	{
		ImGui::Text("ComponentID: %i", interactable.GetID());
		ImGui::Text("ParentID: %i", interactable.GetParent().GetID());
	}

	ImGui::Button("Add Interaction", ImVec2(ImGui::GetContentRegionAvail().x, 24));
	if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonLeft))
	{
		static std::shared_ptr<MenuComponent> selectedComponent;
		static InteractionType selectedType = InteractionType::None;

		const char* componentPreviewValue = selectedComponent ? selectedComponent->myName.c_str() : "(None)";
		if (ImGui::BeginCombo("Component", componentPreviewValue))
		{
			ImGui::SeparatorText("Sprites");
			auto spriteComponents = aObject.GetComponents<SpriteComponent>();
			for (size_t i = 0; i < spriteComponents.size(); i++)
			{
				std::string previewValue = std::to_string(spriteComponents[i]->GetID()) + " " + spriteComponents[i]->myName;
				const bool isSelected = (selectedComponent == spriteComponents[i]);
				if (ImGui::Selectable(previewValue.c_str(), isSelected))
				{
					selectedComponent = spriteComponents[i];
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::SeparatorText("Texts");
			auto textComponents = aObject.GetComponents<TextComponent>();
			for (size_t i = 0; i < textComponents.size(); i++)
			{
				std::string previewValue = std::to_string(textComponents[i]->GetID()) + " " + textComponents[i]->myName;
				const bool isSelected = (selectedComponent == textComponents[i]);
				if (ImGui::Selectable(previewValue.c_str(), isSelected))
				{
					selectedComponent = textComponents[i];
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		const char* interactionPreviewValue = InteractionTypes[(int)selectedType];
		if (ImGui::BeginCombo("Interaction type", interactionPreviewValue))
		{
			for (int j = 0; j < IM_ARRAYSIZE(InteractionTypes); j++)
			{
				const bool isSelected = ((int)selectedType == j);
				if (ImGui::Selectable(InteractionTypes[j], isSelected))
				{
					selectedType = (InteractionType)j;
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if (ImGui::Button("Add", ImVec2(120, 0)))
		{
			interactable.AddInteraction(selectedComponent, selectedType);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	for (size_t i = 0; i < interactable.myInteractions.size(); i++)
	{
		std::shared_ptr<Interaction> interaction = interactable.myInteractions[i];
		ImGui::Text("Component: %i %s", (int)interaction->myParent->GetID(), interaction->myParent->myName.c_str());

		ImGui::Text("Interaction: %s", InteractionTypes[(int)interaction->myType]);

		switch (interaction->myType)
		{
		case MENU::InteractionType::Drag:
		{
			std::shared_ptr<DragInteraction> drag = std::static_pointer_cast<DragInteraction>(interaction);
			ImGui::DragFloat("Min", &drag->myMin, 1.f, 0.f, 0.f, "%.0f");
			ImGui::DragFloat("Max", &drag->myMax, 1.f, 0.f, 0.f, "%.0f");
			ImGui::DragFloat("Value", &drag->myValue, 0.01f, 0.f, 1.f, "%.2f");

				break;
		}
		case MENU::InteractionType::Clip:
		{

			break;
		}
		case MENU::InteractionType::Hide:
		{

			break;
		}
		default:
			break;
		}
	}

	if (ImGui::TreeNode("More"))
	{
		if (ImGui::Button("RemoveComponent", ImVec2(ImGui::GetContentRegionAvail().x, 24)))
			aObject.RemoveComponent(interactable.GetID());

		ImGui::TreePop();
	}

	ImGui::Spacing();
	ImGui::PopID();
}
