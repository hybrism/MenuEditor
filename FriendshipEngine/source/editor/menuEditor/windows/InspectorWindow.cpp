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

			sprite.SetTexture(aContext.textures[0]);
		}
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

	ImGui::DragFloat2("Texture size", &texSize.x, 1.f, 0.f, 0.f, "%.f", ImGuiSliderFlags_NoInput);

	ImGui::Separator();

	ImGui::DragFloat2("Position", &sprite.GetPosition().x);
}
