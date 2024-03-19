#include "ObjectHierarchyWindow.h"

#include <engine/utility/Error.h>
#include <engine/graphics/GraphicsEngine.h>

#include <shared/postMaster/PostMaster.h>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <game/gui/ObjectManager.h>
#include <game/gui/MenuHandler.h>
#include <game/gui/MenuObject.h>
#include <game/gui/components/SpriteComponent.h>
#include <game/gui/components/Collider2DComponent.h>
#include <game/gui/components/TextComponent.h>
#include <game/gui/components/CommandComponent.h>

MENU::MenuObjectHierarchy::MenuObjectHierarchy(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{
	mySelectedStateID = 0;
	mySelectedObjectID = INVALID_ID;
	myRightClickedStateID = INVALID_ID;
	myRightClickedObjectID = INVALID_ID;

	Vector2i center = GraphicsEngine::GetInstance()->DX().GetViewportDimensions() / 2;
	myViewportCenter = { (float)center.x, (float)center.y };
}

void MENU::MenuObjectHierarchy::Show(const MenuEditorUpdateContext& aContext)
{
	if (!myData.isOpen)
		return;

	//ImGui::SetNextWindowSize(ImVec2(350.f, 350.f), ImGuiCond_Appearing);
	//ImGui::SetNextWindowPos(ImVec2(1560.f, 45.f), ImGuiCond_Appearing);
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

			for (unsigned int menuIndex = 0; menuIndex < states.size(); menuIndex++)
			{
				std::string displayName = std::to_string(menuIndex) + " " + states[menuIndex].name;
				if (ImGui::Selectable(displayName.c_str(), states[menuIndex].id == mySelectedStateID))
				{
					mySelectedStateID = states[menuIndex].id;
					mySelectedObjectID = INVALID_ID;
					PushMenuObjectToInspector(mySelectedObjectID);
					aContext.menuHandler->PushState(mySelectedStateID);

					FE::PostMaster::GetInstance()->SendMessage({ FE::eMessageType::UpdateMenuEditorColliders, mySelectedStateID });
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup("Menu State Edit");
					myRightClickedStateID = states[menuIndex].id;
				}
			}

			if (ImGui::BeginPopup("Menu State Edit"))
			{
				if (aContext.showDebugData)
					ImGui::Text("Item pressed: %i", myRightClickedStateID);

				ImGui::InputText("Name", &aContext.menuHandler->GetCurrentState().name, ImGuiInputTextFlags_AutoSelectAll);

				if (ImGui::Selectable("Remove"))
				{
					aContext.menuHandler->RemoveState(myRightClickedStateID);


					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

		}
		ImGui::EndChild();

		ImGui::SeparatorText("Objects");

		AddObjectButton(aContext);

		if (ImGui::BeginChild("MenuObjects", ImGui::GetContentRegionAvail(), true))
		{
			auto& states = aContext.menuHandler->GetAllStates();
			if (states.empty())
			{
				ImGui::EndChild();
				ImGui::End();
				return;
			}

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
				if (aContext.showDebugData)
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
						PushMenuObjectToInspector(INVALID_ID);

					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void MENU::MenuObjectHierarchy::PushMenuObjectToInspector(unsigned int aID)
{
	FE::PostMaster::GetInstance()->SendMessage({ FE::eMessageType::PushEntityToInspector, aID });
}

void MENU::MenuObjectHierarchy::AddStateButton(const MenuEditorUpdateContext& aContext)
{
	UNREFERENCED_PARAMETER(aContext);

	if (ImGui::Button("Add new Sub-Menu", ImVec2(ImGui::GetContentRegionAvail().x, 24)))
		myNewStateName = "(Untitled)";

	if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonLeft))
	{
		ImGui::InputText("Name", &myNewStateName, ImGuiInputTextFlags_AutoSelectAll);
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

void MENU::MenuObjectHierarchy::AddObjectButton(const MenuEditorUpdateContext& aContext)
{
	UNREFERENCED_PARAMETER(aContext);

	ImGui::Button("Add new Object", ImVec2(ImGui::GetContentRegionAvail().x, 24));
	if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonLeft))
	{
		if (ImGui::Selectable("Add Empty"))
		{
			aContext.menuHandler->CreateNewObject(myViewportCenter);
			FE::PostMaster::GetInstance()->SendMessage({ FE::eMessageType::UpdateMenuEditorColliders, this });
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Selectable("Add Button"))
		{
			MenuObject& obj = aContext.menuHandler->CreateNewObject(myViewportCenter);

			obj.AddComponent<SpriteComponent>();
			obj.AddComponent<Collider2DComponent>();
			obj.AddComponent<TextComponent>();

			FE::PostMaster::GetInstance()->SendMessage({ FE::eMessageType::UpdateMenuEditorColliders, this });
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}
