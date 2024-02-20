#include "InspectorWindow.h"

#include <shared/postMaster/PostMaster.h>
#include <imgui/imgui.h>

ME::InspectorWindow::InspectorWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{}

void ME::InspectorWindow::Show(const UpdateContext&)
{
	if (!myData.isOpen)
		return;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{

	}
	ImGui::End();
}
