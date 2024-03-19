#include "pch.h"
#include "CameraSystem.h"

#include <ecs/World.h>

#include "../component/PlayerComponent.h"
#include "../component/TransformComponent.h"
#include "../component/CameraComponent.h"

#include <engine/utility/InputManager.h>

#ifndef _RELEASE
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
#ifdef _EDITOR
	InputManager::GetInstance()->SetMouseMode(DirectX::Mouse::MODE_ABSOLUTE); // _EDITOR
#else
	InputManager::GetInstance()->SetMouseMode(DirectX::Mouse::MODE_RELATIVE);
#endif
}


void CameraSystem::Update(const SceneUpdateContext& dt)
{
	dt;
	auto* ge = GraphicsEngine::GetInstance();

	if (ge->IsViewCameraInUse()) { return; }

//#ifndef _RELEASE
//	if (InputManager::GetInstance()->IsKeyPressed(VK_F1))
//	{
//		if (ge->GetCamera() == &myDebugCamera.myCamera)
//			ge->ResetToViewCamera();
//		else
//			ge->ChangeCurrentCamera(&myDebugCamera.myCamera);
//
//		return;
//	}
//
//	if (ge->GetCamera() == &myDebugCamera.myCamera)
//	{
//		myDebugCamera.Update(dt);
//		return;
//	}
//#endif

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

			//cameraHeight = 0;
			transformComponent.transform.SetPosition(Vector3f((float)footPosition.x, (float)footPosition.y + playerComponent.currentCameraHeight, (float)footPosition.z));

			camera.GetTransform().SetPosition(transformComponent.transform.GetPosition());
			camera.GetTransform().SetEulerAngles({ playerComponent.cameraRotation.x, playerComponent.cameraRotation.y, 0 });
			//MouseLean(camera, playerComponent.cameraRotation);
		
		}
#ifndef _RELEASE

		//TODO: IS THIS ALLOWED?

		else
		{
			CameraComponent& maCamira = myWorld->GetComponent<CameraComponent>(entity);

			Camera& camera = *ge->GetCamera();
			camera.GetTransform().SetPosition(maCamira.Pos);
			camera.GetTransform().SetEulerAngles(maCamira.Rot);

		}
#endif
	}
}

#include <iostream>
void CameraSystem::MouseLean(Camera& camera, const Vector2f& aCameraRotation)
{
	aCameraRotation;
	float currentRot = aCameraRotation.y;
	static float lastRot;
	float lean = currentRot - lastRot;
	static float leanTowards = 0;

	if (lean > 0)
	{
		leanTowards -= 1.5f;
	}
	else if (lean < 0)
	{
		leanTowards += 1.5f;
	}




	lastRot = currentRot;



	camera.GetTransform().SetEulerAngles({ aCameraRotation.x, aCameraRotation.y, leanTowards });
}


void CameraSystem::RunWiggle()
{
}

void CameraSystem::IdleWiggle()
{
}


