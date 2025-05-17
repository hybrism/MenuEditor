#pragma once
#include <engine/debug/DebugCamera.h>

class FreeCameraHandler
{
public:
	void Initialize();

	void ActivatePlayMode();
	void ActivateFreeCameraMode();
	void SwitchToFreeCamera();
	void UpdateFreeCamera(float dt);

private:
	DebugCamera myDebugCamera;
};