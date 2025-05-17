#include "FreeCameraHandler.h"
#include <engine/graphics/GraphicsEngine.h>
#include <engine/utility/Error.h>
#include <engine/utility/InputManager.h>
#include <engine/Engine.h>

void FreeCameraHandler::Initialize()
{
	auto camera = GraphicsEngine::GetInstance()->GetCamera();
	camera->GetTransform().SetPosition({ 0, 1000, -3000.0f });
	camera->GetTransform().SetEulerAngles({ 25, 0, 0 });
}

void FreeCameraHandler::ActivatePlayMode()
{

	GraphicsEngine::GetInstance()->ResetToViewCamera();
	GraphicsEngine::GetInstance()->SetUseOfFreeCamera(false);
	Error::DebugPrintInfo("Deacivate Debug Camera");
	InputManager::GetInstance()->LockMouseScreen(Engine::GetInstance()->GetWindowHandle());
}

void FreeCameraHandler::ActivateFreeCameraMode()
{
	memcpy(&myDebugCamera, GraphicsEngine::GetInstance()->GetCamera(), sizeof(Camera));
	GraphicsEngine::GetInstance()->ChangeCurrentCamera(&myDebugCamera.myCamera);
	GraphicsEngine::GetInstance()->SetUseOfFreeCamera(true);
	Error::DebugPrintInfo("Debug Camera Active");
}

void FreeCameraHandler::SwitchToFreeCamera()
{
	auto* ge = GraphicsEngine::GetInstance();

	if (InputManager::GetInstance()->IsKeyPressed(VK_F1))
	{
		if (ge->IsFreeCameraInUse())
		{
			ActivatePlayMode();
		}
		else
		{
			ActivateFreeCameraMode();
		}
		return;
	}
}

void FreeCameraHandler::UpdateFreeCamera(float dt)
{
	auto* ge = GraphicsEngine::GetInstance();
	SwitchToFreeCamera();


	if (ge->IsFreeCameraInUse())
	{
		if (InputManager::GetInstance()->IsKeyHeld(VK_RBUTTON))
		{
			myDebugCamera.Update(dt);
			InputManager::GetInstance()->LockMouseScreen(Engine::GetInstance()->GetWindowHandle());
		}
		else
		{
			myDebugCamera.SetSkipFrameUpdateBool(true);
			InputManager::GetInstance()->UnlockMouseScreen();
		}
		return;
	}

}
