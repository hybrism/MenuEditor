#include "pch.h"
#include "CameraSystem.h"

#include <ecs/World.h>

#include "../component/PlayerComponent.h"
#include "../component/TransformComponent.h"
#include "../component/CameraComponent.h"

#include <engine/utility/InputManager.h>

#include <engine\math\Math.h>
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


void CameraSystem::Update(SceneUpdateContext& aSceneContext)
{
	auto* ge = GraphicsEngine::GetInstance();

	if (ge->IsFreeCameraInUse()) { return; }

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

	for (Entity entity : myEntities)
	{
		Entity playerEntity = myWorld->GetPlayerEntityID();

		if (playerEntity != INVALID_ENTITY)
		{
			PlayerComponent& playerComponent = myWorld->GetComponent<PlayerComponent>(playerEntity);
			TransformComponent& playerTransform = myWorld->GetComponent<TransformComponent>(playerEntity);
			CameraComponent& cameraComponent = myWorld->GetComponent<CameraComponent>(entity);
			TransformComponent& transformComponent = myWorld->GetComponent<TransformComponent>(entity);

			Camera& camera = *ge->GetCamera();
			
			transformComponent.transform = playerTransform.transform;
		//	MouseLean(camera, playerComponent.cameraRotation);
			//SmoothCamera(camera, playerComponent, transformComponent, cameraComponent , dt);

			SmoothCamera(camera, playerComponent, transformComponent, cameraComponent, aSceneContext);

			HandleGodSpeed(aSceneContext, camera, playerComponent);


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
	float currentRot = aCameraRotation.y;
	static float lastRot;
	float lean = currentRot - lastRot;

	float result = std::max(-6.f, std::min(myLeanTowards, 6.f));

	if (lean >= 0.f)
	{
		if (result <= 6)
		{

			myLeanTowards -= 1.5f;
		}
	}
	else if (lean <= 0.f)
	{
		if (result >= -6)
		{
			myLeanTowards += 1.5f;
		}

	}
	lastRot = currentRot;
	PrintF(std::to_string(lean), std::to_string(result));
		
	camera.GetTransform().SetEulerAngles({ aCameraRotation.x, aCameraRotation.y, result });
}



void CameraSystem::SmoothCamera(Camera& camera, const PlayerComponent& aPlayerComponent, TransformComponent& cameraTransform, CameraComponent& aCameraComponent , const SceneUpdateContext& aContext)
{
	auto footPosition = aPlayerComponent.controller->getFootPosition();

	cameraTransform.transform.SetPosition(Vector3f((float)footPosition.x, (float)footPosition.y + aPlayerComponent.currentCameraHeight, (float)footPosition.z));


	myPlayerVelocity = aPlayerComponent.finalVelocity.Length();

	if (camera.GetHeadBobBool())
	{
		if (aPlayerComponent.currentPlayerState == ePlayerClassStates::Ground)
		{
			aCameraComponent.headBobData.velocityIntensity = 0.1f;

			//headBobIntensity
			aCameraComponent.headBobData.maxVelocity = std::min(myPlayerVelocity, 4.f);

			aCameraComponent.headBobData.headBobSpeedPosY = 7.5f;

			//Yaw
			aCameraComponent.headBobData.bobSpeedYaw = 3.5f;
			//Pitch
			aCameraComponent.headBobData.PitchBobSpeed = 7.5f;
		}

		if (myPlayerVelocity >= 1)
		{
			float headBobIntensity = aCameraComponent.headBobData.maxVelocity * aCameraComponent.headBobData.velocityIntensity;
			aCameraComponent.headBobData.myCameraPositionYOffset = headBobIntensity * sin(aCameraComponent.headBobData.headBobSpeedPosY * (float)aContext.totalTime);

			float yBobOffset = headBobIntensity * sin(aCameraComponent.headBobData.bobSpeedYaw * (float)aContext.totalTime);
			aCameraComponent.headBobData.myCameraRotationY = aPlayerComponent.cameraRotation.y + yBobOffset;

			float xBobOffset = headBobIntensity * sin(aCameraComponent.headBobData.PitchBobSpeed * (float)aContext.totalTime);
			aCameraComponent.headBobData.myCameraRotationX = aPlayerComponent.cameraRotation.x + xBobOffset;
		}
		else
		{
			float headBobIntensity = 6.f * aCameraComponent.headBobData.velocityIntensity;
			aCameraComponent.headBobData.myCameraPositionYOffset = headBobIntensity * sin(3.5f * (float)aContext.totalTime);

			float xBobIntensity = 2.f * aCameraComponent.headBobData.velocityIntensity;
			float xBobOffset = xBobIntensity * sin(3.5f * (float)aContext.totalTime);

			aCameraComponent.headBobData.myCameraRotationX = aPlayerComponent.cameraRotation.x + xBobOffset;
			aCameraComponent.headBobData.myCameraRotationY = aPlayerComponent.cameraRotation.y;

		}
	}
	else
	{
		aCameraComponent.headBobData.myCameraPositionYOffset = 0;
		aCameraComponent.headBobData.myCameraRotationY = aPlayerComponent.cameraRotation.y;
		aCameraComponent.headBobData.myCameraRotationX = aPlayerComponent.cameraRotation.x;
		aCameraComponent.headBobData.myCameraRotationZ = 0;
	}

	if (aPlayerComponent.currentPlayerState == ePlayerClassStates::Wallrun)
	{
		WallRunning(camera, aPlayerComponent, aCameraComponent);

	}
	else if (aPlayerComponent.currentPlayerState == ePlayerClassStates::Slide)
	{
		SlideCamera(camera, aPlayerComponent, aCameraComponent);
	}
	else if(myPlayerVelocity > 10.2f && camera.GetHeadBobBool())
	{

		const float lerpSpeedA = 0.08f;
		const float lerpSpeedB = 3.0f;
		const float ClampLean = 0.10f;

		static float oldPlayerRotation = 0.0f;
		float newPlayerRotation = aCameraComponent.headBobData.myCameraRotationY;
		float deltaPlayerRotation = newPlayerRotation - oldPlayerRotation;

		float currentAngleA = camera.GetTransform().GetEulerRotation().z;

		if (std::abs(deltaPlayerRotation) > 0.2f )
		{
			float leanOffset = (deltaPlayerRotation > 0) ? -1.0f : 1.0f;
			float targetAngleA = currentAngleA + deltaPlayerRotation * lerpSpeedA * aContext.dt + leanOffset;

			float maxPositiveLean = currentAngleA + ClampLean;
			float maxNegativeLean = currentAngleA - ClampLean;
			float interpolatedAngleA = std::min(std::max(targetAngleA, maxNegativeLean), maxPositiveLean);

			aCameraComponent.headBobData.myCameraRotationZ = interpolatedAngleA;
		}
		else if (std::abs(deltaPlayerRotation) < 0.2f)
		{
		
			float targetAngle = 0.0f;
			float interpolatedAngleA = currentAngleA + (targetAngle - currentAngleA) * lerpSpeedB * aContext.dt;

			aCameraComponent.headBobData.myCameraRotationZ = interpolatedAngleA;
		}

		oldPlayerRotation = newPlayerRotation;
	}
	else
	{
		const float lerpSpeedB = 3.0f;

		float currentAngleA = camera.GetTransform().GetEulerRotation().z;

		float targetAngle = 0.0f;
		float interpolatedAngleA = currentAngleA + (targetAngle - currentAngleA) * lerpSpeedB * aContext.dt;

		aCameraComponent.headBobData.myCameraRotationZ = interpolatedAngleA;
	}

	//float currentPos = cameraTransform.transform.GetPosition().y;
	aCameraComponent.headBobData.myCameraPositionY = cameraTransform.transform.GetPosition().y;
	camera.GetTransform().SetPosition({ cameraTransform.transform.GetPosition().x, aCameraComponent.headBobData.myCameraPositionY + aCameraComponent.headBobData.myCameraPositionYOffset, cameraTransform.transform.GetPosition().z });
	camera.GetTransform().SetEulerAngles({ aCameraComponent.headBobData.myCameraRotationX, aCameraComponent.headBobData.myCameraRotationY, aCameraComponent.headBobData.myCameraRotationZ });

}

void CameraSystem::SlideCamera(Camera& camera, const PlayerComponent& /*aPlayerComponent*/, CameraComponent& aCameraComponent)
{
	const float lerpSpeed = 0.1f;

	float targetAngle = 1.f;

	float currentAngle = camera.GetTransform().GetEulerRotation().z;
	float interpolatedAngle = currentAngle + (targetAngle - currentAngle) * lerpSpeed;
	aCameraComponent.headBobData.myCameraRotationZ = interpolatedAngle;
}
void CameraSystem::WallRunning(Camera& camera, const PlayerComponent& aPlayerComponent, CameraComponent& aCameraComponent)
{

	const float lerpSpeed = 0.1f;

	float targetAngle = (aPlayerComponent.isWallRunningDirection == 1) ? -10.f : 10.f;

	float currentAngle = camera.GetTransform().GetEulerRotation().z;
	float interpolatedAngle = currentAngle + (targetAngle - currentAngle) * lerpSpeed;
	aCameraComponent.headBobData.myCameraRotationZ = interpolatedAngle;


}
void CameraSystem::RunWiggle()
{
}

void CameraSystem::IdleWiggle()
{
}
#include <engine/graphics/PostProcess.h>
void CameraSystem::HandleGodSpeed(const SceneUpdateContext& aSceneContext, Camera& aCamera, const PlayerComponent& aPlayerComponent)
{
	static float timerToGodSpeed = 0;
	static float timerToNormal = 0;
	static float timeToGodspeed = 0.15f;
	static float timeToNormal = 1.f;
	if (aPlayerComponent.isGodSpeed)
	{
		timerToGodSpeed = std::min(timeToGodspeed, timerToGodSpeed + aSceneContext.dt);
		float fovProgress = FriendMath::Lerp(100, 120, timerToGodSpeed / timeToGodspeed);
		aSceneContext.postProcess->GetBufferData().speedLineRadiusAddition = FriendMath::Lerp(20, 0, timerToGodSpeed / timeToGodspeed);
		aCamera.SetFOV(fovProgress);
		timerToNormal = 0;
	}
	else
	{
		if (aCamera.GetFOW() == 100)
			return;
		timerToNormal = std::min(timeToNormal, timerToNormal + aSceneContext.dt);
		float fovProgress = FriendMath::Lerp(aCamera.GetFOW(), 100, timerToNormal / timeToNormal);
		auto& ref = aSceneContext.postProcess->GetBufferData();
		ref.speedLineRadiusAddition = FriendMath::Lerp(0, 20, timerToNormal / timeToNormal);
		aCamera.SetFOV(fovProgress);
		timerToGodSpeed = 0;
	}
}


