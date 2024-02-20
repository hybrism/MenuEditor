#include "MenuObjectHierarchy.h"

#include <shared/postMaster/PostMaster.h>
#include <imgui/imgui.h>

#include "../gui/MenuHandler.h"

ME::MenuObjectHierarchy::MenuObjectHierarchy(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{}

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
								

				ImGui::Text("Object!");

				//TODO: Om ett object är selectat, skicka det till inspectorwindow?
			}
			ImGui::EndChild();
		}
	}
	ImGui::End();
}
