#pragma once
#include <engine/debug/DebugCamera.h>

class CameraSystem : public System<CameraSystem>
{
public:
	CameraSystem(World* aWorld);
	~CameraSystem() override;

	void Init() override;
	void Update(const SceneUpdateContext& dt) override;

	void MouseLean(Camera& camera, const Vector2f& aCameraRotation);
	void RunWiggle();
	void IdleWiggle();
private:
	void HandleLockMouse();
	bool isLocked = false;
	DebugCamera myDebugCamera;
};