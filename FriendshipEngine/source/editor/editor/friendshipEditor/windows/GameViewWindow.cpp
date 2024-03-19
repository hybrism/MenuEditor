#include "GameViewWindow.h"

#include "../EditorManager.h" // TODO: MOVE THIS TO A BETTER PLACE
#include <engine/utility/InputManager.h>
#include <engine/Engine.h>
#include <engine/graphics/GraphicsEngine.h>
#include <engine/utility/Error.h>
#include <imgui/imgui.h>
#include <game/Game.h>

FE::GameViewWindow::GameViewWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{}

void FE::GameViewWindow::Show(const EditorUpdateContext& aContext)
{
	if (!myData.isOpen)
		return;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		ImVec2 buttonSize = { 0, 20 };
		SceneManager& sceneManager = aContext.game->GetSceneManager();
		
		if (!myShouldPauseFirstFrame) {} //Do nothing
		else
		{
			sceneManager.SetIsPaused(true);
			myShouldPauseFirstFrame = false;
		}

		if (ImGui::IsWindowFocused())
		{
			auto* im = InputManager::GetInstance();
			if (im->IsKeyPressed('T'))
			{
				sceneManager.TogglePaused();
			}

			if (im->IsKeyPressed('R'))
			{
				GraphicsEngine::GetInstance()->ResetToViewCamera();
				GraphicsEngine::GetInstance()->SetUseOfFreeCamera(false);
				sceneManager.ReloadCurrentScene();
				sceneManager.SetIsPaused(false);
			}
		}

		if (ImGui::Button("Play", buttonSize))
		{
			InputManager::GetInstance()->LockMouseScreen(Engine::GetInstance()->GetWindowHandle());
			sceneManager.SetIsPaused(false);
		}

		ImGui::SameLine();
		if (ImGui::Button("Pause", buttonSize))
		{
			sceneManager.SetIsPaused(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop", buttonSize))
		{
			sceneManager.ReloadCurrentScene();
			myShouldPauseFirstFrame = true;
		}

		ImGui::SameLine();
		ImGui::Text(" ( %s ) \t", aContext.game->GetSceneManager().GetIsPaused() ? myGameStates[0] : myGameStates[1]);
		ImGui::SameLine();
		ImGui::Text("[T]oggle Play/Pause - [R]eload Scene - [F1] Toggle DebugCamera ");

		ImVec2 region = ImGui::GetContentRegionAvail();
		Vector2i dimensions = GraphicsEngine::GetInstance()->DX().GetViewportDimensions();

		float aspectRatio = (float)dimensions.x / (float)dimensions.y;
		if (region.x / region.y > aspectRatio)
		{
			region.x = region.y * aspectRatio;
		}
		else
		{
			region.y = region.x / aspectRatio;
		}

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImVec2(vMin.x + region.x, vMin.y + region.y);
		ImVec2 wPos = ImGui::GetWindowPos();

		ImVec2 vMinWin = wPos + vMin;
		ImVec2 vMaxWin = wPos + vMax;


		vMinWin += buttonSize * 2.f;

		aContext.editorManager->SetGameWindowRect({ vMinWin, vMaxWin });

		const ImVec2 textPos = ImVec2(vMinWin.x + 10, vMinWin.y + 10);
		ImVec2 textOffset = ImVec2(0, 15);

		std::string fps = "FPS: " + std::to_string((int)(1.0f / aContext.dt));
		std::string drawCall = "Draw Calls: " + std::to_string(GraphicsEngine::GetInstance()->GetDrawCalls());

		ImDrawList* tDrawList = ImGui::GetWindowDrawList();
		tDrawList->AddImage(GraphicsEngine::GetInstance()->GetBackBufferSRV().Get(), vMinWin, vMaxWin);
		tDrawList->AddText(textPos, IM_COL32_WHITE, fps.c_str());
		tDrawList->AddText(textPos + textOffset, IM_COL32_WHITE, drawCall.c_str());
	}
	ImGui::End();
}
