#include "AssetsWindow.h"
//#include "../gui/IDManager.h"

#include <d3d11.h>
#include <engine/graphics/Texture.h>
#include <shared/postMaster/PostMaster.h>
#include <imgui/imgui.h>

MENU::AssetsWindow::AssetsWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{}

void MENU::AssetsWindow::Show(const MenuEditorUpdateContext& aContext)
{
	if (!myData.isOpen)
		return;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		if (ImGui::BeginChild("Assets", ImGui::GetContentRegionAvail(), true))
		{
			if (ImGui::TreeNode("Sprites"))
			{
				for (size_t i = 0; i < aContext.assets.textures.size(); i++)
				{
					std::string textureName = aContext.assets.textureIdToName[i];

					ImGui::MenuItem(textureName.c_str());
					if (ImGui::BeginItemTooltip())
					{
						Vector2f texSize = aContext.assets.textures[i]->GetTextureSize();
						ImGui::Image(aContext.assets.textures[i]->GetShaderResourceView(), ImVec2(texSize.x, texSize.y));

						ImGui::EndTooltip();
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Fonts"))
			{
				for (size_t i = 0; i < aContext.assets.fontFiles.size(); i++)
				{
					std::string fontFile = aContext.assets.fontFiles[i];

					ImGui::MenuItem(fontFile.c_str());
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("IDs"))
			{
				//auto ids = IDManager::GetInstance()->GetIDs();
				//for (size_t i = 0; i < ids.size(); i++)
				//{
				//	std::string status = ids[i] ? "Free" : "-";
				//	ImGui::Text("ID: %i %s", i, status.c_str());
				//}
				ImGui::TreePop();
			}

			ImGui::EndChild();
		}
	}
	ImGui::End();
}
