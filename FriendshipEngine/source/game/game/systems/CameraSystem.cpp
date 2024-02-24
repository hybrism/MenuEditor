#include "pch.h"
#include "CameraSystem.h"

#include <ecs/World.h>

#include "../component/PlayerComponent.h"
#include "../component/TransformComponent.h"
#include "../component/CameraComponent.h"

#include <engine/utility/InputManager.h>

#ifdef _DEBUG
#include <engine/debug/DebugCamera.h>
#endif


CameraSystem::CameraSystem(World* aWorld) : System(aWorld)
{
	ComponentSignature CameraSystemSignature;
	CameraSystemSignature.set(myWorld->GetComponentSignatureID<TransformComponent>());
	CameraSystemSignature.set(myWorld->GetComponentSignatureID<CameraComponent>());
	aWorld->SetSystemSignature<CameraSystem>(CameraSystemSignature);

	memcpy(&myDebugCamera, GraphicsEngine::GetInstance()->GetViewCamera(), sizeof(Camera));
}

CameraSystem::~CameraSystem()
{
	ClipCursor(nullptr);
}

//const float MOVE_SPEED = 400.0f;
//const float MOVE_MULTIPLIER = 3.0f;
//const float CAMERA_MOVE_X = 8.0f;
//const float CAMERA_MOVE_Y = 5.0f;
//bool ACTIVE_MULTIPLIER = false;

#include <engine/Engine.h>
#include <engine/graphics/GraphicsEngine.h>
#include <engine/utility/Error.h>

void CameraSystem::Init()
{
	//InputManager::GetInstance()->LockMouseScreen(Engine::GetInstance()->GetWindowHandle());
	InputManager::GetInstance()->SetMouseMode(DirectX::Mouse::MODE_RELATIVE);
}


void CameraSystem::Update(const float& dt)
{
	dt;
	HandleLockMouse();

	if (isLocked) { return; }

	auto* ge = GraphicsEngine::GetInstance();

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsKeyPressed(VK_F1))
	{
		if (ge->GetCamera() == &myDebugCamera.myCamera)
			ge->ResetToViewCamera();
		else
			ge->ChangeCurrentCamera(&myDebugCamera.myCamera);

		return;
	}

	if (ge->GetCamera() == &myDebugCamera.myCamera)
	{
		myDebugCamera.Update(dt);
		return;
	}
#endif

	for (auto& entity : myEntities)
	{
		Entity playerEntity = myWorld->GetPlayerEntityID();

		if (playerEntity != INVALID_ENTITY)
		{
			PlayerComponent& playerComponent = myWorld->GetComponent<PlayerComponent>(playerEntity);
			TransformComponent& playerTransform = myWorld->GetComponent<TransformComponent>(playerEntity);

			TransformComponent& transformComponent = myWorld->GetComponent<TransformComponent>(entity);

			Camera& camera = *ge->GetCamera();

			transformComponent.transform = playerTransform.transform;

			auto footPosition = playerComponent.controller->getFootPosition();

			float cameraHeight = (playerComponent.isCrouching || playerComponent.isSliding) ? playerComponent.cameraCrouchHeight : playerComponent.cameraHeight;
			//cameraHeight = 0;
			transformComponent.transform.SetPosition(Vector3f((float)footPosition.x, (float)footPosition.y + cameraHeight, (float)footPosition.z));

			camera.GetTransform().SetPosition(transformComponent.transform.GetPosition());
			camera.GetTransform().SetEulerAngles({ playerComponent.cameraRotation.x, playerComponent.cameraRotation.y, 0 });
		}
	}
}

void CameraSystem::HandleLockMouse()
{
	InputManager* im = InputManager::GetInstance();
	if (im->IsKeyPressed(VK_ESCAPE))
	{
		if (!isLocked)
			im->UnlockMouseScreen();
		else
			im->LockMouseScreen(Engine::GetInstance()->GetWindowHandle());

		isLocked = !isLocked;
	}
}

