#include "MenuObjectHierarchy.h"

#include <engine/utility/Error.h>
#include <shared/postMaster/PostMaster.h>
#include <imgui/imgui.h>

#include "../gui/MenuHandler.h"
#include "../gui/menuObject/MenuObject.h"

ME::MenuObjectHierarchy::MenuObjectHierarchy(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{
	mySelectedIndex = INT_MAX;
}

void ME::MenuObjectHierarchy::Show(const UpdateContext& aContext)
{
	if (!myData.isOpen)
		return;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		if (ImGui::Button("Add Empty Object"))
		{
			aContext.menuHandler->myObjectManager.CreateNew();
		}

		if (ImGui::BeginChild("MenuObjects", ImGui::GetContentRegionAvail(), true))
		{
			for (size_t i = 0; i < aContext.menuHandler->myObjectManager.myObjects.size(); i++)
			{
				MenuObject& object = aContext.menuHandler->myObjectManager.myObjects[i];
				
				std::string displayName = std::to_string(i) + " " + object.GetName();
				if (ImGui::Selectable(displayName.c_str(), i == mySelectedIndex))
				{
					mySelectedIndex = i;
					PushMenuObjectToInspector();
				}
			}
			ImGui::EndChild();
		}
	}
	ImGui::End();
}

void ME::MenuObjectHierarchy::PushMenuObjectToInspector()
{
	FE::PostMaster::GetInstance()->SendMessage({ FE::eMessageType::PushEntityToInspector, mySelectedIndex });
}
