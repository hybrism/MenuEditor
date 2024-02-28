#include "MenuObjectHierarchy.h"

#include <engine/utility/Error.h>
#include <engine/graphics/GraphicsEngine.h>

#include <shared/postMaster/PostMaster.h>
#include <imgui/imgui.h>

#include "../gui/ObjectManager.h"
#include "../gui/MenuHandler.h"
#include "../gui/MenuObject.h"

MENU::MenuObjectHierarchy::MenuObjectHierarchy(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{
	mySelectedID = UINT_MAX;
	myRightClickedID = UINT_MAX;
	Vector2i center = GraphicsEngine::GetInstance()->GetViewportDimensions() / 2;
	myViewportCenter = { (float)center.x, (float)center.y };
}

void MENU::MenuObjectHierarchy::Show(const UpdateContext& aContext)
{
	if (!myData.isOpen)
		return;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		if (ImGui::Button("Add Empty Object"))
		{
			aContext.menuHandler->CreateNewObject(myViewportCenter);
		}

		if (ImGui::BeginChild("MenuObjects", ImGui::GetContentRegionAvail(), true))
		{
			for (unsigned int i = 0; i < aContext.menuHandler->GetObjectsSize(); i++)
			{
				MenuObject& object = aContext.menuHandler->GetObjectFromIndex(i);

				std::string displayName = std::to_string(i) + " " + object.GetName();
				if (ImGui::Selectable(displayName.c_str(), aContext.menuHandler->GetObjectFromIndex(i).GetID() == mySelectedID))
				{
					mySelectedID = object.GetID();
					PushMenuObjectToInspector(mySelectedID);
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup("Menu Object Edit");
					myRightClickedID = aContext.menuHandler->GetObjectFromIndex(i).GetID();
				}
			}

			if (ImGui::BeginPopup("Menu Object Edit"))
			{
				ImGui::Text("Item pressed: %i", myRightClickedID);

				if (ImGui::Selectable("Move Up"))
				{
					aContext.menuHandler->MoveUpObjectAtID(myRightClickedID);
				}

				if (ImGui::Selectable("Move Down"))
				{
					aContext.menuHandler->MoveDownObjectAtID(myRightClickedID);
				}

				if (ImGui::Selectable("Remove"))
				{
					aContext.menuHandler->RemoveObjectAtID(aContext.menuHandler->GetObjectFromID(myRightClickedID).GetID());

					if (myRightClickedID == mySelectedID)
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
