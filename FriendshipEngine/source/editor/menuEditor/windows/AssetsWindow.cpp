#include "AssetsWindow.h"
#include <engine/graphics/Texture.h>
#include <shared/postMaster/PostMaster.h>
#include <imgui/imgui.h>

ME::AssetsWindow::AssetsWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{}

void ME::AssetsWindow::Show(const UpdateContext& aContext)
{
	if (!myData.isOpen)
		return;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		for (size_t i = 0; i < aContext.textures.size(); i++)
		{
			ImGui::SameLine();
			ImGui::Image(aContext.textures[i]->GetShaderResourceView(), ImVec2(64, 64));
		}
	}
	ImGui::End();
}
