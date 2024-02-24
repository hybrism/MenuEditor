#include "InspectorWindow.h"
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <game/gui/MenuObject.h>
#include <game/gui/MenuHandler.h>
#include <game/gui/ObjectManager.h>

//#include "../UpdateContext.h"

#include <game/gui/components/SpriteComponent.h>
#include <game/gui/components/TextComponent.h>

#include <shared/postMaster/PostMaster.h>

ME::InspectorWindow::InspectorWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{
	mySelectedIndex = INT_MAX;
	myIsNewObjectSelected = false;

	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::PushEntityToInspector);
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::NewMenuLoaded);
}

void ME::InspectorWindow::Show(const UpdateContext& aContext)
{
	aContext;
	if (!myData.isOpen)
		return;

	if (myIsNewObjectSelected)
	{
		//Get values from object
		//myObjectName = aContext.menuHandler->myObjectManager.myObjects[mySelectedIndex]->GetName();

		myIsNewObjectSelected = false;
	}

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		if (mySelectedIndex == INT_MAX) //Early out
		{
			ImGui::End();
			return;
		}

		//MenuObject& selectedObject = *aContext.menuHandler->myObjectManager.myObjects[mySelectedIndex];
		//ImGui::PushID(selectedObject.myID);

		//ImGui::InputText("##", &myObjectName);
		//ImGui::SameLine();
		//if (ImGui::Button("Save Name"))
		//{
		//	selectedObject.SetName(myObjectName);
		//}

		//if (selectedObject.HasComponent<SpriteComponent>())
		//	EditSpriteComponent(aContext, selectedObject);

		//if (selectedObject.HasComponent<TextComponent>())
		//	EditTextComponent(selectedObject);

		if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 24)))
		{
			//Display popup where you can select which component to add

			//SpriteComponent& sprite = selectedObject.AddComponent<SpriteComponent>();

			//sprite.SetTexture(aContext.textures[0], aContext.textureIDtoPath[0]);

			//TextComponent& text = selectedObject.AddComponent<TextComponent>();
			//text.SetText("Hello World!");
		}

		ImGui::PopID();
	}
	ImGui::End();
}

void ME::InspectorWindow::RecieveMessage(const FE::Message& aMessage)
{
	switch (aMessage.myEventType)
	{
	case FE::eMessageType::PushEntityToInspector:
	{
		mySelectedIndex = std::any_cast<size_t>(aMessage.myMessage);
		myIsNewObjectSelected = true;
		break;
	}
	case FE::eMessageType::NewMenuLoaded:
	{
		mySelectedIndex = INT_MAX;
		break;
	}
	default:
		break;
	}
}

void ME::InspectorWindow::EditSpriteComponent(const UpdateContext& aContext, MenuObject& aObject)
{
	aContext;

	SpriteComponent& sprite = aObject.GetComponent<SpriteComponent>();
	sprite;
	//ImGui::PushID("Sprite");
	//ImGui::SeparatorText("Sprite");

	//Vector2f texSize = sprite.GetTextureSize();
	//ImGui::Text("Texture size x: %i y: %i", (int)texSize.x, (int)texSize.y);
	//Texture* currentItem = sprite.GetTexture();

	//if (ImGui::BeginCombo("Select Texture", sprite.GetTexturePath().c_str()))
	//{
	//	for (size_t i = 0; i < aContext.textures.size(); i++)
	//	{
	//		bool isSelected = (currentItem == aContext.textures[i]);

	//		if (ImGui::Selectable(aContext.textureIDtoPath[i].c_str(), isSelected))
	//			sprite.SetTexture(aContext.textures[i], aContext.textureIDtoPath[i]);

	//		if (isSelected)
	//			ImGui::SetItemDefaultFocus();
	//	}

	//	ImGui::EndCombo();
	//}

	//ImGui::Spacing();

	//ImGui::DragFloat2("Position", &sprite.GetPosition().x);
	//ImGui::DragFloat2("Size", &sprite.GetSize().x, 0.01f);
	//ImGui::DragFloat2("Pivot", &sprite.GetPivot().x, 0.001f, 0.f, 1.f);
	//ImGui::DragFloat2("ScaleMultiplier", &sprite.GetScaleMultiplier().x);
	//ImGui::ColorEdit4("Color", &sprite.GetColor().x);

	////ImGui::DragFloat("Rotation", &sprite.GetRotation(), 0.01f);
	//ClipValue& clip = sprite.GetClipValue();
	//ImGui::SetNextItemWidth(50.f);
	//ImGui::DragFloat("Left", &clip.left, 0.01f, 0.f, 1.f);
	//ImGui::SameLine();
	//ImGui::SetNextItemWidth(50.f);
	//ImGui::DragFloat("Right", &clip.right, 0.01f, 0.f, 1.f);
	//ImGui::SetNextItemWidth(50.f);
	//ImGui::DragFloat("Upper", &clip.upper, 0.01f, 0.f, 1.f);
	//ImGui::SameLine();
	//ImGui::SetNextItemWidth(50.f);
	//ImGui::DragFloat("Down", &clip.down, 0.01f, 0.f, 1.f);
	//ImGui::Checkbox("Is Hidden", &sprite.GetIsHidden());

	ImGui::PopID();
}

void ME::InspectorWindow::EditTextComponent(MenuObject& aObject)
{
	TextComponent& text = aObject.GetComponent<TextComponent>();
	text;

	//ImGui::PushID("Text");
	//ImGui::SeparatorText("Text");

	//std::string string = text.GetText();
	//Vector2f position = text.GetPosition();
	//Vector4f color = text.GetColor();

	//if (ImGui::InputText("Text", &string))
	//	text.SetText(string);

	//if (ImGui::DragFloat2("Position", &position.x))
	//	text.SetPosition(position);

	//if (ImGui::ColorEdit4("Color", &color.x))
	//	text.SetColor(color);

	//ImGui::PopID();
}
