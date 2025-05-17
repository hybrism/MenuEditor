#include "pch.h"

#include <variant>

#include "../../audio/NewAudioManager.h"

#include "CommandComponent.h"
#include "CommandFunctions.h"

#include <engine/utility/Error.h>
#include <engine/graphics/Camera.h>
#include <engine/graphics/GraphicsEngine.h>
#include <engine/Engine.h>

#include "../MenuObject.h"


MENU::CommandComponent::CommandComponent(MenuObject& aParent, unsigned int aID)
	: MenuComponent(aParent, aID, ComponentType::Command)
{
	myCommandType = eCommandType::Count;
}

void MENU::CommandComponent::Execute(CommandData aData, const MenuUpdateContext& aContext)
{
#ifdef _RELEASE
	NewAudioManager::GetInstance()->PlaySound(eSounds::MenyNavigation, 0.5f);
#endif

	if (!myCommand)
	{
		PrintW("No command set!");
		return;
	}

	myCommand(aData, aContext);
}

void MENU::CommandComponent::SetCommandType(eCommandType aType)
{
	myCommandType = aType;

	switch (myCommandType)
	{
	case MENU::eCommandType::PopMenu:
		myCommand = PopMenuCommand;
		break;
	case MENU::eCommandType::PushMenu:
		myCommand = PushMenuCommand;
		break;
	case MENU::eCommandType::LoadLevel:
		myCommand = LoadSceneCommand;
		break;
	case MENU::eCommandType::ResumeGame:
		myCommand = ResumeGameCommand;
		break;
	case MENU::eCommandType::BackToMainMenu:
		myCommand = ReturnToMainMenuCommand;
		break;
	case MENU::eCommandType::QuitGame:
		myCommand = QuitGameCommand;
		break;
	case MENU::eCommandType::Headbob:
	{
		myCommand = HeadbobCommand;

		auto& camera = *GraphicsEngine::GetInstance()->GetCamera();
		myCommandData.data = &camera.GetHeadBobBool();
		break;
	}
	case MENU::eCommandType::Fullscreen:
		myCommand = FullscreenCommand;

		myCommandData.data = &Engine::GetInstance()->myIsFullscreen;
		break;
	case MENU::eCommandType::Resolution:
		myCommand = ResolutionCommand;

		myCommandData.data = &GraphicsEngine::GetInstance()->GetInternalResolution();
		break;
	case MENU::eCommandType::SfxVolume:
		myCommand = SfxVolumeCommand;
		break;
	case MENU::eCommandType::MusicVolume:
		myCommand = MusicVolumeCommand;
		break;
	default:
		break;
	}
}

void MENU::CommandComponent::SetCommandData(CommandData aData)
{
	myCommandData = aData;
}

MENU::eCommandType MENU::CommandComponent::GetCommandType() const
{
	return myCommandType;
}

MENU::CommandData MENU::CommandComponent::GetCommandData()
{
	return myCommandData;
}

