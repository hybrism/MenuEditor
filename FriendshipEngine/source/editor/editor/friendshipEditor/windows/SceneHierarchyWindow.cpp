#include "SceneHierarchyWindow.h"

#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <shared/postMaster/PostMaster.h>
#include <ecs/World.h>
#include <game/component/MetaDataComponent.h>
#include <format>

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

				if (mySceneHierarchySearchWord.size() > 0)
				{
					std::string displayName = data.name;

					if (displayName.find(mySceneHierarchySearchWord) == std::string::npos) { continue; }
				}

				bool result = false;

				ImGui::PushID((int)entity);

				if (data.size == 0)
				{
					result = ImGui::Selectable("##", mySelectedEntity == entity);
				}
				else
				{
				std::string name = "[" + std::to_string(entity) + "] ";
				name += data.name;
					result = ImGui::Selectable(name.c_str(), mySelectedEntity == entity);
				}
				ImGui::PopID();

				if (result)
				{
					mySelectedEntity = entity;
					PushInspectorWindow(entity);
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
