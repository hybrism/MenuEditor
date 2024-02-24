#pragma once
#include <engine/debug/DebugCamera.h>

class CameraSystem : public System
{
public:
	CameraSystem(World* aWorld);
	~CameraSystem() override;

	void Init() override;
	void Update(const float& dt) override;
private:
	void HandleLockMouse();
	bool isLocked = false;
	DebugCamera myDebugCamera;
};