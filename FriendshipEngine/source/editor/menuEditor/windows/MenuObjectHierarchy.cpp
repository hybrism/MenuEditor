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
		ImGui::SeparatorText("MenuStates");
		AddStateButton(aContext);

		if (ImGui::BeginChild("MenuStates", ImVec2(ImGui::GetContentRegionAvail().x, 80.f), true))
		{
			auto& states = aContext.menuHandler->GetAllStates();
			if (states.empty())
			{
				ImGui::EndChild();
				ImGui::End();
				return;
			}

			mySelectedStateID = aContext.menuHandler->GetCurrentState().id;

			for (size_t i = 0; i < states.size(); i++)
			{
				std::string displayName = std::to_string(i) + " " + states[i].name;
				if (ImGui::Selectable(displayName.c_str(), states[i].id == mySelectedStateID))
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

		ImGui::SeparatorText("Objects");

		AddObjectButton(aContext);

		if (ImGui::BeginChild("MenuObjects", ImGui::GetContentRegionAvail(), true))
		{
			if (aContext.menuHandler->GetAllStates().empty())
				ImGui::EndChild();

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

void MENU::MenuObjectHierarchy::AddStateButton(const UpdateContext& aContext)
{
	if (ImGui::Button("Add new Sub-Menu", ImVec2(ImGui::GetContentRegionAvail().x, 24)))
		myNewStateName = "(Untitled)";

	if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonLeft))
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

void MENU::MenuObjectHierarchy::AddObjectButton(const UpdateContext& aContext)
{
	if (ImGui::Button("Add new Object", ImVec2(ImGui::GetContentRegionAvail().x, 24)))
	{
	}

	if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonLeft))
	{
		if (ImGui::Button("Add empty", ImVec2(120, 0)))
		{
			aContext.menuHandler->CreateNewObject(myViewportCenter);
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
