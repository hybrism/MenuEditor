#include "DeferredViewWindow.h"
#include <engine/graphics/GraphicsEngine.h>
#include <engine/graphics/DirectionalLightManager.h>
#include <imgui/imgui.h>

FE::DeferredViewWindow::DeferredViewWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{}

void FE::DeferredViewWindow::Show(const EditorUpdateContext& aContext)
{
	aContext;

	if (!myData.isOpen)
		return;

	auto* ge = GraphicsEngine::GetInstance();
	auto& gBuffer = ge->GetGBuffer();

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		if (ImGui::BeginTable("Defferred", 2, ImGuiTableFlags_BordersInner))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("#0 World Position");
			ImGui::Image(gBuffer.mySRVs[0].Get(), ImVec2(256, 144));

			ImGui::TableSetColumnIndex(1);
			ImGui::Text("#1 Albedo");
			ImGui::Image(gBuffer.mySRVs[1].Get(), ImVec2(256, 144));

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("#2 Normal");
			ImGui::Image(gBuffer.mySRVs[2].Get(), ImVec2(256, 144));

			ImGui::TableSetColumnIndex(1);
			ImGui::Text("#3 Vertex Normal");
			ImGui::Image(gBuffer.mySRVs[3].Get(), ImVec2(256, 144));

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("#4 Material");
			ImGui::Image(gBuffer.mySRVs[4].Get(), ImVec2(256, 144));

			ImGui::TableSetColumnIndex(1);
			ImGui::Text("#5 Emissive");
			ImGui::Image(gBuffer.mySRVs[5].Get(), ImVec2(256, 144));

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("DepthBuffer");
			ImGui::Image(ge->GetDepthBuffer().SRV.Get(), ImVec2(256, 144));


			ImGui::EndTable();
		}
		ImGui::Spacing();
	}
	ImGui::End();
}
