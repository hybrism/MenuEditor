#include "MenuViewWindow.h"

#include <engine/graphics/GraphicsEngine.h>
#include <imgui/imgui.h>

ME::MenuViewWindow::MenuViewWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{}

void ME::MenuViewWindow::Show(const UpdateContext&)
{
	if (!myData.isOpen)
		return;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		ImVec2 region = ImGui::GetContentRegionAvail();
		Vector2i dimensions = GraphicsEngine::GetInstance()->GetViewportDimensions();

		float aspectRatio = (float)dimensions.x / (float)dimensions.y;
		if (region.x / region.y > aspectRatio)
		{
			region.x = region.y * aspectRatio;
		}
		else
		{
			region.y = region.x / aspectRatio;
		}

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImVec2(vMin.x + region.x, vMin.y + region.y);
		ImVec2 wPos = ImGui::GetWindowPos();

		ImVec2 vMinWin = wPos + vMin;
		ImVec2 vMaxWin = wPos + vMax;

		ImDrawList* tDrawList = ImGui::GetWindowDrawList();
		tDrawList->AddImage(GraphicsEngine::GetInstance()->GetBackBufferSRV().Get(), vMinWin, vMaxWin);
	}
	ImGui::End();
}
