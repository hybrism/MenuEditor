#include "pch.h"
#include "CameraSystem.h"

#include <ecs/World.h>
#include <engine/graphics/Camera.h>

#include "../component/PlayerComponent.h"
#include "../component/TransformComponent.h"
#include "../component/CameraComponent.h"

#include <engine/utility/InputManager.h>


CameraSystem::CameraSystem(World* aWorld) : System(aWorld)
{
	ComponentSignature CameraSystemSignature;
	CameraSystemSignature.set(myWorld->GetComponentSignatureID<TransformComponent>());
	CameraSystemSignature.set(myWorld->GetComponentSignatureID<CameraComponent>());
	aWorld->SetSystemSignature<CameraSystem>(CameraSystemSignature);
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


void CameraSystem::Update(const float&)
{
	HandleLockMouse();

	if (isLocked) { return; }

	for (auto& entity : myEntities)
	{
		Entity playerEntity = myWorld->GetPlayerEntityID();

		if (playerEntity != INVALID_ENTITY)
		{
			PlayerComponent& playerComponent = myWorld->GetComponent<PlayerComponent>(playerEntity);
			TransformComponent& playerTransform = myWorld->GetComponent<TransformComponent>(playerEntity);

			TransformComponent& transformComponent = myWorld->GetComponent<TransformComponent>(entity);

			Camera& camera = *GraphicsEngine::GetInstance()->GetCamera();

			transformComponent.transform = playerTransform.transform;

			auto footPosition = playerComponent.controller->getFootPosition();

			float cameraHeight = (playerComponent.isCrouching || playerComponent.isSliding) ? playerComponent.cameraCrouchHeight : playerComponent.cameraHeight;
			//cameraHeight = 0;
			transformComponent.transform.SetPosition(Vector3f((float)footPosition.x, (float)footPosition.y + cameraHeight, (float)footPosition.z));

			camera.SetPosition(transformComponent.transform.GetPosition());
			camera.SetRotation({ playerComponent.cameraRotation.x, playerComponent.cameraRotation.y, 0 });
		}
	}
}

void CameraSystem::HandleLockMouse()
{
	InputManager* im = InputManager::GetInstance();
	if (im->IsKeyPressed(VK_TAB))
	{
		if (!isLocked)
			im->UnlockMouseScreen();
		else
			im->LockMouseScreen(Engine::GetInstance()->GetWindowHandle());

		isLocked = !isLocked;
	}
}

