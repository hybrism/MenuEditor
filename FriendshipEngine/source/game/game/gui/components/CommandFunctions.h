#pragma once
#include "CommandComponent.h"

namespace MENU
{
	struct MenuUpdateContext;

	void LoadSceneCommand(MENU::CommandData aData, const MENU::MenuUpdateContext& aContext);
	void PushMenuCommand(MENU::CommandData aData, const MENU::MenuUpdateContext& aContext);
	void PopMenuCommand(MENU::CommandData aData, const MENU::MenuUpdateContext& aContext);
	void ResumeGameCommand(MENU::CommandData aData, const MENU::MenuUpdateContext& aContext);
	void ReturnToMainMenuCommand(MENU::CommandData aData, const MENU::MenuUpdateContext& aContext);
	void QuitGameCommand(MENU::CommandData aData, const MENU::MenuUpdateContext& aContext);

}