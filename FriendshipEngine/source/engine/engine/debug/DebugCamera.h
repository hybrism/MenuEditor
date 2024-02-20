#pragma once


class Camera;

class DebugCamera
{
public:
	static void DebugCameraControl(Camera& aCamera, float aDeltaTime);

	static const float MOVE_SPEED;
	static const float MOVE_MULTIPLIER;
	static const float CAMERA_MOVE_X;
	static const float CAMERA_MOVE_Y;
	static bool ACTIVE_MULTIPLIER;
private:
	static void MoveForward(Camera& aCamera, float aDeltaTime);
	static void MoveBackward(Camera& aCamera, float aDeltaTime);
	static void MoveLeft(Camera& aCamera, float aDeltaTime);
	static void MoveRight(Camera& aCamera, float aDeltaTime);
	static void MoveUp(Camera& aCamera, float aDeltaTime);
	static void MoveDown(Camera& aCamera, float aDeltaTime);
	static void RotateCameraWithMouse(Camera& aCamera, float aDeltaTime);

};