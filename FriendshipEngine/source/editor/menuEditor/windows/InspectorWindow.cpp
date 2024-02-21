#include "InspectorWindow.h"
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include "../gui/menuObject/MenuObject.h"
#include "../gui/MenuHandler.h"

#include "../UpdateContext.h"
#include "../gui/components/SpriteComponent.h"

#include <shared/postMaster/PostMaster.h>

ME::InspectorWindow::InspectorWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{
	mySelectedIndex = INT_MAX;
	myNewObjectSelected = false;
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::PushEntityToInspector);
}

void ME::InspectorWindow::Show(const UpdateContext& aContext)
{
	if (!myData.isOpen)
		return;

	if (myNewObjectSelected)
	{
		//Get values from object
		myObjectName = aContext.menuHandler->myObjectManager.myObjects[mySelectedIndex].GetName();

		myNewObjectSelected = false;
	}

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		if (mySelectedIndex == INT_MAX) //Early out
		{
			ImGui::End();
			return;
		}

		MenuObject& selectedObject = aContext.menuHandler->myObjectManager.myObjects[mySelectedIndex];
		ImGui::PushID(selectedObject.myID);

		ImGui::InputText("##", &myObjectName);
		ImGui::SameLine();
		if (ImGui::Button("Save Name"))
		{
			selectedObject.SetName(myObjectName);
		}

		if (selectedObject.HasComponent<SpriteComponent>())
		{

			EditSpriteData(selectedObject);

		}

		if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 24)))
		{
			//Display popup where you can select which component to add

			SpriteComponent& sprite = selectedObject.AddComponent<SpriteComponent>();

			sprite.SetTexture(aContext.textures[0], aContext.textureIDtoPath[0]);
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
		myNewObjectSelected = true;
		break;
	}
	default:
		break;
	}
}

void ME::InspectorWindow::EditSpriteData(MenuObject& aObject)
{

	SpriteComponent& sprite = aObject.GetComponent<SpriteComponent>();

	ImGui::SeparatorText("Sprite");

	Vector2f texSize = sprite.GetTextureSize();
	ImGui::Text("Texture size x: %i y: %i", (int)texSize.x, (int)texSize.y);

	ImGui::Spacing();

	ImGui::DragFloat2("Position", &sprite.GetPosition().x);
	ImGui::DragFloat2("Size", &sprite.GetSize().x, 0.01f);
	ImGui::DragFloat2("Pivot", &sprite.GetPivot().x, 0.001f, 0.f, 1.f);
	ImGui::DragFloat2("ScaleMultiplier", &sprite.GetScaleMultiplier().x);
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

}
