#pragma once
#include <memory>
#include <string>
#include <imgui/imgui.h>
#include "../UpdateContext.h"

namespace ME
{
	enum class ID
	{
		MenuView,
		MenuObjectHierarchy,
		Inspector,
		Assets,
		Console,
		Count
	};

	struct WindowData
	{
		std::string handle = "N/A";
		bool isOpen = false;
		ImGuiWindowFlags flags = ImGuiWindowFlags_None;
	};

	class WindowBase
	{
	public:
		WindowBase(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
		{
			myData.handle = aHandle;
			myData.isOpen = aOpen;
			myData.flags = aFlags;
		}

		WindowData myData = {};
		virtual void Show(const UpdateContext& aContext) = 0;
	};
};