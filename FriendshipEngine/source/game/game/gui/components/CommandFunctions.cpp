#include "pch.h"
#include "CommandFunctions.h"

#include "../MenuHandler.h"
#include "scene/Scene.h"
#include "../../scene/SceneManager.h"

namespace MENU
{
	void LoadSceneCommand(MENU::CommandData aData, const MENU::MenuUpdateContext& aContext)
	{
		std::string levelName = std::get<std::string>(aData.data);

		if (!aContext.sceneManager)
		{
			Print("Will load level: " + levelName);
			return;
		}

		aContext.sceneManager->LoadScene({ eSceneType::Game, levelName });
	}

	void PushMenuCommand(MENU::CommandData aData, const MENU::MenuUpdateContext& aContext)
	{
		ID menuID = std::get<ID>(aData.data);
		aContext.menuHandler->PushState(menuID);
	}

	void PopMenuCommand(MENU::CommandData aData, const MENU::MenuUpdateContext& aContext)
	{
		UNREFERENCED_PARAMETER(aData);

		aContext.menuHandler->PopState();
	}

	void ResumeGameCommand(MENU::CommandData aData, const MENU::MenuUpdateContext& aContext)
	{
		UNREFERENCED_PARAMETER(aData);

		if (!aContext.sceneManager)
		{
			Print("Resume Game");
			return;
		}

		aContext.sceneManager->SetIsPaused(false);
	}

	void ReturnToMainMenuCommand(MENU::CommandData aData, const MENU::MenuUpdateContext& aContext)
	{
		UNREFERENCED_PARAMETER(aData);

		if (!aContext.sceneManager)
		{
			Print("Return to MainMenu");
			return;
		}

		while (aContext.sceneManager->GetCurrentScene()->GetType() != eSceneType::MainMenu)
			aContext.sceneManager->PopScene();
	}

	void QuitGameCommand(MENU::CommandData aData, const MENU::MenuUpdateContext& aContext)
	{
		UNREFERENCED_PARAMETER(aData);
		
		if (!aContext.sceneManager)
		{
			Print("Quit Game!");
			return;
		}
		
		while (aContext.sceneManager->GetCurrentScene()->GetType() != eSceneType::MainMenu)
			aContext.sceneManager->PopScene();

		aContext.sceneManager->PopScene();
	}
}