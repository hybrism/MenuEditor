#include "MenuObjectHierarchy.h"

#include <engine/utility/Error.h>
#include <engine/graphics/GraphicsEngine.h>

#include <shared/postMaster/PostMaster.h>
#include <imgui/imgui.h>

#include "../gui/ObjectManager.h"
#include "../gui/MenuHandler.h"
#include "../gui/MenuObject.h"

MENU::MenuObjectHierarchy::MenuObjectHierarchy(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{
	mySelectedIndex = INT_MAX;
	Vector2i center = GraphicsEngine::GetInstance()->GetViewportDimensions() / 2;
	myViewportCenter = { (float)center.x, (float)center.y };
}

void MENU::MenuObjectHierarchy::Show(const UpdateContext& aContext)
{
	if (!myData.isOpen)
		return;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		if (ImGui::Button("Add Empty Object"))
		{
			aContext.menuHandler->CreateNewObject(myViewportCenter);
		}

		if (ImGui::BeginChild("MenuObjects", ImGui::GetContentRegionAvail(), true))
		{
			for (size_t i = 0; i < aContext.menuHandler->GetObjectsSize(); i++)
			{
				MenuObject& object = aContext.menuHandler->GetObjectFromID(i);

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

void MENU::MenuObjectHierarchy::PushMenuObjectToInspector()
{
	FE::PostMaster::GetInstance()->SendMessage({ FE::eMessageType::PushEntityToInspector, mySelectedIndex });
}
