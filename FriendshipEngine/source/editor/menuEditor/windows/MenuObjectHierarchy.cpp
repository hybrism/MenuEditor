#include "MenuObjectHierarchy.h"

#include <engine/utility/Error.h>
#include <engine/graphics/GraphicsEngine.h>

#include <shared/postMaster/PostMaster.h>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include "../gui/ObjectManager.h"
#include "../gui/MenuHandler.h"
#include "../gui/MenuObject.h"

MENU::MenuObjectHierarchy::MenuObjectHierarchy(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{
	mySelectedObjectID = UINT_MAX;
	myRightClickedObjectID = UINT_MAX;
	mySelectedStateID = 0;
	Vector2i center = GraphicsEngine::GetInstance()->GetViewportDimensions() / 2;
	myViewportCenter = { (float)center.x, (float)center.y };
}

void MENU::MenuObjectHierarchy::Show(const UpdateContext& aContext)
{
	if (!myData.isOpen)
		return;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		if (ImGui::Button("Add New State"))
		{
			ImGui::OpenPopup("Add State");
			myNewStateName = "(Untitled)";
		}

		AddStatePopup(aContext);

		if (ImGui::BeginChild("MenuStates", ImVec2(ImGui::GetContentRegionAvail().x, 80.f), true))
		{
			auto& states = aContext.menuHandler->GetAllStates();

			for (size_t i = 0; i < states.size(); i++)
			{
				if (ImGui::Selectable(states[i].name.c_str(), states[i].id == mySelectedStateID))
				{
					mySelectedStateID = states[i].id;
					mySelectedObjectID = UINT_MAX;
					PushMenuObjectToInspector(mySelectedObjectID);
					aContext.menuHandler->PushState(mySelectedStateID);
					FE::PostMaster::GetInstance()->SendMessage({ FE::eMessageType::UpdateEditorColliders, mySelectedStateID });
				}
			}

			ImGui::EndChild();
		}

		if (ImGui::Button("Add Empty Object"))
		{
			aContext.menuHandler->CreateNewObject(myViewportCenter);
		}

		if (ImGui::BeginChild("MenuObjects", ImGui::GetContentRegionAvail(), true))
		{
			MenuState& state = aContext.menuHandler->GetCurrentState();

			for (unsigned int objectIndex = 0; objectIndex < state.objectIds.size(); objectIndex++)
			{
				MenuObject& object = aContext.menuHandler->GetObjectFromID(state.objectIds[objectIndex]);

				std::string displayName = std::to_string(objectIndex) + " " + object.GetName();
				if (ImGui::Selectable(displayName.c_str(), aContext.menuHandler->GetObjectFromIndex(objectIndex).GetID() == mySelectedObjectID))
				{
					mySelectedObjectID = object.GetID();
					PushMenuObjectToInspector(mySelectedObjectID);
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup("Menu Object Edit");
					myRightClickedObjectID = aContext.menuHandler->GetObjectFromIndex(objectIndex).GetID();
				}
			}

			if (ImGui::BeginPopup("Menu Object Edit"))
			{
				ImGui::Text("Item pressed: %i", myRightClickedObjectID);

				if (ImGui::Selectable("Move Up"))
				{
					aContext.menuHandler->MoveUpObjectAtID(myRightClickedObjectID);
				}

				if (ImGui::Selectable("Move Down"))
				{
					aContext.menuHandler->MoveDownObjectAtID(myRightClickedObjectID);
				}

				if (ImGui::Selectable("Remove"))
				{
					aContext.menuHandler->RemoveObjectAtID(aContext.menuHandler->GetObjectFromID(myRightClickedObjectID).GetID());

					if (myRightClickedObjectID == mySelectedObjectID)
						PushMenuObjectToInspector(UINT_MAX);

					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			ImGui::EndChild();
		}
	}
	ImGui::End();
}

void MENU::MenuObjectHierarchy::PushMenuObjectToInspector(unsigned int aID)
{
	FE::PostMaster::GetInstance()->SendMessage({ FE::eMessageType::PushEntityToInspector, aID });
}

void MENU::MenuObjectHierarchy::AddStatePopup(const UpdateContext& aContext)
{
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Add State", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("Name", &myNewStateName);
		ImGui::Spacing();

		if (ImGui::Button("Add", ImVec2(120, 0)))
		{
			aContext.menuHandler->AddNewState(myNewStateName);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}


}
