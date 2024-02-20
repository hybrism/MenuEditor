#include "InspectorWindow.h"

#include <imgui/imgui.h>
#include <engine/utility/Error.h>
#include <shared/postMaster/PostMaster.h>

FE::InspectorWindow::InspectorWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{
	PostMaster::GetInstance()->Subscribe(this, eMessageType::PushEntityToInspector);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::NewLevelLoaded);
}

void FE::InspectorWindow::Show(const EditorUpdateContext& aContext)
{
	if (!myData.isOpen)
		return;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		if (myInspectorWindows.empty() || aContext.world == nullptr) //Early out
		{
			ImGui::End();
			return;
		}

		ImGui::PushID(myInspectorWindows[0].GetWindowID());
		if (ImGui::BeginChild("Entity"))
		{
			myInspectorWindows[0].Show(aContext);
		}
		ImGui::EndChild();
		ImGui::PopID();

		if (myInspectorWindows.size() > 1) //Handle extra pop-up Windows
		{
			for (size_t i = 1; i < myInspectorWindows.size(); i++)
			{
				WindowData& data = myInspectorWindows[i].myData;
				ImGui::PushID(myInspectorWindows[i].GetWindowID());
				if (ImGui::Begin(data.handle.c_str(), &data.isOpen, data.flags))
				{
					myInspectorWindows[i].Show(aContext);
				}
				ImGui::End();
				ImGui::PopID();

				if (!myInspectorWindows[i].myData.isOpen)
					myInspectorWindows.erase(myInspectorWindows.begin() + i);
			}
		}
	}
	ImGui::End();
}

void FE::InspectorWindow::Clear()
{
	myInspectorWindows.clear();
	myWindowIdCounter = 0;
}

void FE::InspectorWindow::RecieveMessage(const Message& aMessage)
{
	switch (aMessage.myEventType)
	{
	case eMessageType::PushEntityToInspector:
	{
		Entity entity = std::any_cast<Entity>(aMessage.myMessage);

		for (int i = 0; i < myInspectorWindows.size(); i++)
		{
			//If window with entityID is already open, return
			if (myInspectorWindows[i].GetEntityID() == entity)
				return;
		}

		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || myInspectorWindows.empty())
		{
			myInspectorWindows.push_back(Inspector(entity, myWindowIdCounter++));
		}
		else
		{
			myInspectorWindows[0].SetEntityID(entity);
		}
		break;
	}
	case eMessageType::NewLevelLoaded:
	{
		Clear();
		break;
	}
	default:
		break;
	}
}