#include "pch.h"
#include "CommandFunctions.h"

#include "TextComponent.h"
#include "../MenuHandler.h"
#include "../MenuObject.h"
#include "scene/Scene.h"
#include "../../scene/SceneManager.h"
#include "../../audio/NewAudioManager.h"

#include "../engine/engine/graphics/GraphicsEngine.h"
#include "../engine/engine/graphics/Light/LightManager.h"
#include "../engine/engine/graphics/PostProcess.h"
#include "../engine/engine/graphics/Camera.h"

#include "../engine/engine/Engine.h"

namespace MENU
{
	void LoadSceneCommand(CommandData aData, const MenuUpdateContext& aContext)
	{
		std::string levelName = std::get<std::string>(aData.data);

#ifdef _DEBUG
		if (!aContext.sceneManager)
		{
			Print("Will load level: " + levelName);
			return;
		}
#endif

		aContext.menuHandler->PopToBaseState();

		aContext.sceneManager->LoadScene({ eSceneType::Game, levelName });

	}

	void PushMenuCommand(CommandData aData, const MenuUpdateContext& aContext)
	{
		ID menuID = std::get<ID>(aData.data);
		aContext.menuHandler->PushState(menuID);
	}

	void PopMenuCommand(CommandData aData, const MenuUpdateContext& aContext)
	{
		UNREFERENCED_PARAMETER(aData);

		aContext.menuHandler->PopState();
	}

	void ResumeGameCommand(CommandData aData, const MenuUpdateContext& aContext)
	{
		UNREFERENCED_PARAMETER(aData);

#ifdef _DEBUG
		if (!aContext.sceneManager)
		{
			Print("Resume Game");
			return;
		}
#endif
		aContext.sceneManager->SetIsPaused(false);
	}

	void ReturnToMainMenuCommand(CommandData aData, const MenuUpdateContext& aContext)
	{
		UNREFERENCED_PARAMETER(aData);

#ifdef _DEBUG
		if (!aContext.sceneManager)
		{
			Print("Return to MainMenu");
			return;
		}
#endif

		while (aContext.sceneManager->GetCurrentScene()->GetType() != eSceneType::MainMenu)
			aContext.sceneManager->PopScene();
	}

	void QuitGameCommand(CommandData aData, const MenuUpdateContext& aContext)
	{
		UNREFERENCED_PARAMETER(aData);

#ifdef _DEBUG
		if (!aContext.sceneManager)
		{
			Print("Quit Game!");
			return;
		}
#endif

		while (aContext.sceneManager->GetCurrentScene()->GetType() != eSceneType::MainMenu)
			aContext.sceneManager->PopScene();

		aContext.sceneManager->PopScene();
	}

	void HeadbobCommand(CommandData aData, const MenuUpdateContext& aContext)
	{
		UNREFERENCED_PARAMETER(aContext);

		bool* headbob = std::get<bool*>(aData.data);

		*headbob = !*headbob;
	}

	void FullscreenCommand(CommandData aData, const MenuUpdateContext& aContext)
	{
		UNREFERENCED_PARAMETER(aContext);

		GraphicsEngine* ge = GraphicsEngine::GetInstance();

		bool* fullscreen = std::get<bool*>(aData.data);

		{
			HMONITOR hmon = MonitorFromWindow(Engine::GetInstance()->GetWindowHandle(), MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi = { sizeof(mi) };
			GetMonitorInfo(hmon, &mi);

			int width = mi.rcMonitor.right - mi.rcMonitor.left;
			int height = mi.rcMonitor.bottom - mi.rcMonitor.top;

			Vector2i windowDimensions = { width, height };
			aContext.menuHandler->OnResize(windowDimensions);

			MenuObject& resolutionObject = aContext.menuHandler->GetObjectFromName("ResolutionText");
			TextComponent& resolutionText = resolutionObject.GetComponent<TextComponent>();

			size_t resIndex = IM_ARRAYSIZE(resolutions) - 1;
			for (size_t i = 0; i < resIndex; i++)
			{
				if (resolution[i].x == (uint)width && resolution[i].y == (uint)height)
				{
					resIndex = i;
					break;
				}
			}

			resolutionText.SetText(resolutions[resIndex]);
		}


		ge->SetFullscreen(
			!*fullscreen, 
			aContext.lightManager, 
			aContext.postProcess
		);

		//if (fullscreen)
		//{
		//	HMONITOR hmon = MonitorFromWindow(Engine::GetInstance()->GetWindowHandle(), MONITOR_DEFAULTTONEAREST);
		//	MONITORINFO mi = { sizeof(mi) };
		//	GetMonitorInfo(hmon, &mi);

		//	int width = mi.rcMonitor.right - mi.rcMonitor.left;
		//	int height = mi.rcMonitor.bottom - mi.rcMonitor.top;

		//	ge->SetWindowDimensions({ width, height });
		//	//ge->SetInternalResolution(ge->DX().GetViewportDimensions());
		//}
		//else
		//{
		//	ge->SetWindowDimensions(ge->DX().GetWindowDimensions());

		//	//Ska vara selected resolution, måste storas någonstans!
		//	//ge->SetInternalResolution(ge->DX().GetViewportDimensions());
		//}
	}

	void SfxVolumeCommand(CommandData aData, const MenuUpdateContext& aContext)
	{
		UNREFERENCED_PARAMETER(aContext);

		NewAudioManager* audioManager = NewAudioManager::GetInstance();

		float value = std::get<float>(aData.data);

		//FULHACK
		if (value < 0)
			value = -0.099f;
		else
			value = 0.1f;

		float currentVolume = audioManager->GetSfxVolume();
		float newVolume = currentVolume + value;

		if (newVolume > 1.f)
			newVolume = 1.f;

		if (newVolume < 0.f)
			newVolume = 0.f;

#ifdef _DEBUG

		Print("Sfx Volume is at: " + std::to_string(newVolume) + "%");
#endif

		if (newVolume > 1.f || newVolume < 0.f)
			return;

		MenuObject& percentage = aContext.menuHandler->GetObjectFromName("SfxPercentage");
		TextComponent& percentageText = percentage.GetComponent<TextComponent>();

		float percentageFloat = newVolume * 100.f;
		percentageText.SetText(std::to_string(static_cast<int>(percentageFloat)) + " %");



		audioManager->SetSfxVolume(newVolume);
	}

	void MusicVolumeCommand(CommandData aData, const MenuUpdateContext& aContext)
	{
		UNREFERENCED_PARAMETER(aContext);

		NewAudioManager* audioManager = NewAudioManager::GetInstance();

		float value = std::get<float>(aData.data);

		//FULHACK
		if (value < 0)
			value = -0.099f;
		else
			value = 0.1f;

		float currentVolume = audioManager->GetMusicVolume();
		float newVolume = currentVolume + value;

		if (newVolume > 1.f)
			newVolume = 1.f;

		if (newVolume < 0.f)
			newVolume = 0.f;

#ifdef _DEBUG

		Print("Music Volume is at: " + std::to_string(newVolume) + "%");
#endif

		if (newVolume > 1.f || newVolume < 0.f)
			return;

		MenuObject& percentage = aContext.menuHandler->GetObjectFromName("MusicPercentage");
		TextComponent& percentageText = percentage.GetComponent<TextComponent>();

		float percentageFloat = newVolume * 100.f;
		percentageText.SetText(std::to_string(static_cast<int>(percentageFloat)) + " %");


		audioManager->SetMusicVolume(newVolume);
	}

	void ResolutionCommand(CommandData aData, const MenuUpdateContext& aContext)
	{
		if (Engine::GetInstance()->myIsFullscreen) { return; }

		UNREFERENCED_PARAMETER(aContext);

		int value = std::get<int>(aData.data);

		index += value;

		if (index == IM_ARRAYSIZE(resolutions))
			index = 0;
		if (index < 0)
			index = IM_ARRAYSIZE(resolutions) - 1;

		//OnResize() needs to be done BEFORE changing the dimensions in engine
		aContext.menuHandler->OnResize(resolution[index]);

		MenuObject& resolutionObject = aContext.menuHandler->GetObjectFromName("ResolutionText");
		TextComponent& resolutionText = resolutionObject.GetComponent<TextComponent>();

		resolutionText.SetText(resolutions[index]);

		GraphicsEngine::GetInstance()->SetResolution(
			resolution[index],
			aContext.lightManager,
			aContext.postProcess
		);
	}
}