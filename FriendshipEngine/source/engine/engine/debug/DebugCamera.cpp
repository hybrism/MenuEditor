#include "pch.h"
#include "DebugCamera.h"
#include "../graphics/Camera.h"
#include "../utility/InputManager.h"

DebugCamera::DebugCamera(const DebugCameraData& aData) : myData(aData) { }
DebugCamera::~DebugCamera() = default;

float DebugCamera::Multiplier() const
{
	return 1 + myData.myMoveMultiplier * static_cast<int>(myActiveMultiplier);
}

void DebugCamera::Update(float aDeltaTime)
{
	InputManager* im = InputManager::GetInstance();

	// Move
	{
		myActiveMultiplier = im->IsKeyHeld(VK_SHIFT);
		float moveAmount = myData.myMoveSpeed * Multiplier() * aDeltaTime;
		Vector3f moveDirection = { 0, 0, 0 };
		if (im->IsKeyHeld('W'))
		{
			moveDirection += myCamera.GetTransform().GetForward();
		}
		if (im->IsKeyHeld('S'))
		{
			moveDirection -= myCamera.GetTransform().GetForward();
		}
		if (im->IsKeyHeld('A'))
		{
			moveDirection -= myCamera.GetTransform().GetRight();
		}
		if (im->IsKeyHeld('D'))
		{
			moveDirection += myCamera.GetTransform().GetRight();
		}

		if (im->IsKeyHeld('E'))
		{
			moveDirection += myCamera.GetTransform().GetUp();
		}
		if (im->IsKeyHeld('Q'))
		{
			moveDirection -= myCamera.GetTransform().GetUp();
		}

		myCamera.GetTransform().Translate(moveDirection * moveAmount);
	}

	// Look
	{

		Vector2f currentMousePos = im->GetCurrentMousePositionVector2f();


		static Vector2f priorMousePos = currentMousePos;
		Vector2f mouseMovement = priorMousePos - currentMousePos;


		if (mySkipFrameUpdate == true)
		{
			mySkipFrameUpdate = false;
			priorMousePos = currentMousePos;
			return;
		}
		if (abs(mouseMovement.x) > 150 || abs(mouseMovement.y) > 150)
		{
			priorMousePos = currentMousePos;
			return;
		}
	
		myYaw = myCamera.GetTransform().GetEulerRotation().y;
		myPitch = myCamera.GetTransform().GetEulerRotation().x;

		myYaw +=  aDeltaTime * currentMousePos.x * myData.myCameraMoveX;
		myPitch += currentMousePos.y * aDeltaTime * myData.myCameraMoveY ;


		myCamera.GetTransform().SetEulerAngles({ myPitch, myYaw, 0});
		
		priorMousePos = currentMousePos;
	}
}
