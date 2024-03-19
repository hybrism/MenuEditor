#include "ScriptEditorWindow.h"

#include <engine/utility/Error.h>

FE::ScriptEditorWindow::ScriptEditorWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{
	myScriptEditor.Init();
}

void FE::ScriptEditorWindow::Show(const EditorUpdateContext& aContext)
{
	if (!myData.isOpen)
		return;

	ImGui::SetNextWindowSize(ImVec2(1000, 800), ImGuiCond_Appearing);
	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		myScriptEditor.Update(aContext);
	}
	ImGui::End();
}
