#pragma once
#include <memory>
#include <string>
#include <imgui/imgui.h>

#include "../EditorUpdateContext.h"

namespace FE
{
	enum class ID
	{
		SceneHierarchy,
		EditorView,
		GameView,
		Inspector,
		AssetDatabase,
		DeferredView,
		Console,
		ScriptEditor,
		VertexPainter,
		DirectionalLight,
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
		virtual void OnOpen(const EditorUpdateContext&) { __noop; }
		virtual void Show(const EditorUpdateContext& aContext) = 0;
	};
};