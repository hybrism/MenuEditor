#pragma once
#include "CommandComponent.h"
#include <engine/math/Vector.h>

namespace MENU
{
	struct MenuUpdateContext;

	void LoadSceneCommand(CommandData aData, const MenuUpdateContext& aContext);
	void PushMenuCommand(CommandData aData, const MenuUpdateContext& aContext);
	void PopMenuCommand(CommandData aData, const MenuUpdateContext& aContext);
	void ResumeGameCommand(CommandData aData, const MenuUpdateContext& aContext);
	void ReturnToMainMenuCommand(CommandData aData, const MenuUpdateContext& aContext);
	void QuitGameCommand(CommandData aData, const MenuUpdateContext& aContext);
	void HeadbobCommand(CommandData aData, const MenuUpdateContext& aContext);
	void FullscreenCommand(CommandData aData, const MenuUpdateContext& aContext);
	void SfxVolumeCommand(CommandData aData, const MenuUpdateContext& aContext);
	void MusicVolumeCommand(CommandData aData, const MenuUpdateContext& aContext);
	void ResolutionCommand(CommandData aData, const MenuUpdateContext& aContext);

	
	static Vector2ui resolution[3] = { {1600, 900}, {1920,1080}, {2560,1440} };
	static const char* resolutions[3] = { "1600x900", "1920x1080", "2560x1440" };
	static int index = 2;
}