#include "AssetDatabaseWindow.h"

#include <shared/postMaster/PostMaster.h>
#include <imgui/imgui.h>

FE::AssetDatabaseWindow::AssetDatabaseWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{}

void FE::AssetDatabaseWindow::Show(const EditorUpdateContext& aContext)
{
	aContext;

	if (!myData.isOpen)
		return;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{

	}
	ImGui::End();
}
