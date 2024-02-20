#include "ConsoleWindow.h"

#include <shared/postMaster/PostMaster.h>
#include <imgui/imgui.h>

FE::ConsoleWindow::ConsoleWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{
	ClearLog();
	memset(InputBuf, 0, sizeof(InputBuf));
	HistoryPos = -1;

	AutoScroll = true;
	ScrollToBottom = false;
	AddLog("Welcome to FriendshipLog!");

	PostMaster::GetInstance()->Subscribe(this, eMessageType::PrintToConsole);
}

FE::ConsoleWindow::~ConsoleWindow()
{
	ClearLog();
	for (int i = 0; i < History.Size; i++)
		free(History[i]);
}

void FE::ConsoleWindow::Show(const EditorUpdateContext& aContext)
{
	aContext;

	if (!myData.isOpen)
		return;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		if (ImGui::SmallButton("Clear")) { ClearLog(); }
		ImGui::Separator();

		// Reserve enough left-over height for 1 separator + 1 input text
		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), true, ImGuiWindowFlags_HorizontalScrollbar))
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::Selectable("Clear")) ClearLog();
				ImGui::EndPopup();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
			for (const char* item : Items)
			{
				// Normally you would store more information in your item than just a string.
				// (e.g. make Items[] an array of structure, store color/type etc.)
				ImVec4 color;
				bool has_color = false;
				if (strstr(item, "[ERROR]"))
				{
					color = Red;
					has_color = true;
				}
				else if (strstr(item, "[WARNING]"))
				{
					color = Yellow;
					has_color = true;
				}
				else if (strstr(item, "[INFO]"))
				{
					color = Blue;
					has_color = true;
				}
				else if (strncmp(item, "# ", 2) == 0)
				{
					color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
					has_color = true;
				}

				if (has_color)
					ImGui::PushStyleColor(ImGuiCol_Text, color);

				ImGui::TextUnformatted(item);

				if (has_color)
					ImGui::PopStyleColor();
			}

			// Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
			// Using a scrollbar or mouse-wheel will take away from the bottom edge.
			if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
				ImGui::SetScrollHereY(1.0f);
			ScrollToBottom = false;

			ImGui::PopStyleVar();
		}
		ImGui::EndChild();
		ImGui::Separator();

	}
	ImGui::End();
}

void FE::ConsoleWindow::ClearLog()
{
	for (int i = 0; i < Items.Size; i++)
		free(Items[i]);
	Items.clear();
}

void FE::ConsoleWindow::AddLog(const char* fmt, ...) IM_FMTARGS(2)
{
	// FIXME-OPT
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	Items.push_back(Strdup(buf));
}

void FE::ConsoleWindow::RecieveMessage(const Message& aMessage)
{
	switch (aMessage.myEventType)
	{
	case eMessageType::PrintToConsole:
	{
		std::string msg = std::any_cast<std::string>(aMessage.myMessage);
		AddLog(msg.c_str());
		break;
	}
	default:
		break;
	}
}