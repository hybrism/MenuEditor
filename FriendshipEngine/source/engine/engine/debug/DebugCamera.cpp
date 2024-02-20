#include "pch.h"
#include "DebugCamera.h"
#include "../graphics/Camera.h"
#include "../utility/InputManager.h"

const float DebugCamera::MOVE_SPEED = 400.0f;
const float DebugCamera::MOVE_MULTIPLIER = 3.0f;
const float DebugCamera::CAMERA_MOVE_X = 8.0f;
const float DebugCamera::CAMERA_MOVE_Y = 5.0f;
bool DebugCamera::ACTIVE_MULTIPLIER = false;

float Multipler()
{
	return 1 + DebugCamera::MOVE_MULTIPLIER * static_cast<int>(DebugCamera::ACTIVE_MULTIPLIER);
}

void DebugCamera::DebugCameraControl(Camera& aCamera, float aDeltaTime)
{
	ACTIVE_MULTIPLIER = InputManager::GetInstance()->IsKeyHeld(VK_SHIFT);

	if (InputManager::GetInstance()->IsKeyHeld('W'))
	{
		MoveForward(aCamera, aDeltaTime);
	}
	if (InputManager::GetInstance()->IsKeyHeld('S'))
	{
		MoveBackward(aCamera, aDeltaTime);
	}
	if (InputManager::GetInstance()->IsKeyHeld('A'))
	{
		MoveLeft(aCamera, aDeltaTime);
	}
	if (InputManager::GetInstance()->IsKeyHeld('D'))
	{
		MoveRight(aCamera, aDeltaTime);
	}


	if (InputManager::GetInstance()->IsKeyHeld('E'))
	{
		MoveUp(aCamera, aDeltaTime);
	}
	if (InputManager::GetInstance()->IsKeyHeld('Q'))
	{
		MoveDown(aCamera, aDeltaTime);
	}

	//TODO: Fix Rotation with mouse for debug camera!
	//RotateCameraWithMouse(aCamera, aDeltaTime);
}

void DebugCamera::MoveForward(Camera& aCamera, float aDeltaTime)
{
	auto view = aCamera.GetViewMatrix();
	Vector3f direction = { view(3, 1), view(3, 2), view(3, 3) };
	Vector3f newPos = aCamera.GetPosition() + MOVE_SPEED * aDeltaTime * Multipler() * direction;
	aCamera.SetPosition(newPos);
}

void DebugCamera::MoveBackward(Camera& aCamera, float aDeltaTime)
{
	auto view = aCamera.GetViewMatrix();
	Vector3f direction = { view(3, 1), view(3, 2), view(3, 3) };
	Vector3f newPos = aCamera.GetPosition() - MOVE_SPEED * aDeltaTime * Multipler() * direction;
	aCamera.SetPosition(newPos);
}

void DebugCamera::MoveLeft(Camera& aCamera, float aDeltaTime)
{
	auto view = aCamera.GetViewMatrix();
	Vector3f direction = { view(1, 1), view(1, 2), view(1, 3) };
	Vector3f newPos = aCamera.GetPosition() - MOVE_SPEED * aDeltaTime * Multipler() * direction;
	aCamera.SetPosition(newPos);
}

void DebugCamera::MoveRight(Camera& aCamera, float aDeltaTime)
{
	auto view = aCamera.GetViewMatrix();
	Vector3f direction = { view(1, 1), view(1, 2), view(1, 3) };
	Vector3f newPos = aCamera.GetPosition() + MOVE_SPEED * aDeltaTime * Multipler() * direction;
	aCamera.SetPosition(newPos);
}

void DebugCamera::MoveUp(Camera& aCamera, float aDeltaTime)
{
	Vector3f newPos = aCamera.GetPosition();
	newPos.y += MOVE_SPEED * Multipler() * aDeltaTime;
	aCamera.SetPosition(newPos);
}

void DebugCamera::MoveDown(Camera& aCamera, float aDeltaTime)
{
	Vector3f newPos = aCamera.GetPosition();
	newPos.y -= MOVE_SPEED * Multipler() * aDeltaTime;
	aCamera.SetPosition(newPos);
}



void DebugCamera::RotateCameraWithMouse(Camera& aCamera, float aDeltaTime)
{
	static Vector2<float> oldMousePos;
	static float camera_pitch = 0.0;
	static float camera_yaw = 0.0;
	Vector2<float> currentMousePos = InputManager::GetInstance()->GetCurrentMousePositionVector2f();

	if (InputManager::GetInstance()->IsKeyHeld(VK_RBUTTON))
	{
		Vector2<float> mouseMovement = oldMousePos - currentMousePos;

		float rotation_rate_x = CAMERA_MOVE_X * aDeltaTime;
		float rotation_rate_y = CAMERA_MOVE_Y * aDeltaTime;

		camera_yaw -= mouseMovement.x * rotation_rate_x;
		camera_pitch += mouseMovement.y * rotation_rate_y;

		aCamera.SetRotation({ camera_pitch, camera_yaw, 0 });
	}

	oldMousePos = currentMousePos;
}
