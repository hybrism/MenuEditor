#pragma once
#include "Window.h"
#include "../../scriptEditor/ScriptEditor.h"

namespace FE
{
	class ScriptEditorWindow : public WindowBase
	{
	public:
		ScriptEditorWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);

		void Show(const EditorUpdateContext& aContext) override;

	private:
		ScriptEditor myScriptEditor;
	};
}