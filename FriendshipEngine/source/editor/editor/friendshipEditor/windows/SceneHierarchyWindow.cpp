#include "SceneHierarchyWindow.h"

#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <shared/postMaster/PostMaster.h>
#include <ecs/World.h>
#include <game/component/MetaDataComponent.h>

FE::SceneHierarchyWindow::SceneHierarchyWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{
	mySelectedEntity = INVALID_ENTITY;
}

void FE::SceneHierarchyWindow::Show(const EditorUpdateContext& aContext)
{
	if (!myData.isOpen)
		return;

	if (!aContext.world)
		return;

	std::vector<Entity> entities = aContext.world->GetEntities();

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{

		ImGui::SeparatorText("Scene Hierarcy");
		ImGui::InputTextWithHint("Search", "(Case sensitive)", &mySceneHierarchySearchWord);

		ImGui::Text("[EntityID] ObjectName");
		ImGui::Text("Hold left ctrl to open more windows");

		if (ImGui::BeginChild("Entities"))
		{

			for (Entity entity : entities)
			{
				auto& data = aContext.world->GetComponent<MetaDataComponent>(entity);

				std::string displayName = "[" + std::to_string(entity) + "] " + data.name;

				if (displayName.find(mySceneHierarchySearchWord) != std::string::npos)
				{
					if (ImGui::Selectable(displayName.c_str(), mySelectedEntity == entity))
					{
						mySelectedEntity = entity;
						PushInspectorWindow(entity);
					}
				}
			}

			ImGui::EndChild();
		}
	}
	ImGui::End();
}

void FE::SceneHierarchyWindow::PushInspectorWindow(Entity aEntity)
{
	PostMaster::GetInstance()->SendMessage({ eMessageType::PushEntityToInspector, aEntity });
}
