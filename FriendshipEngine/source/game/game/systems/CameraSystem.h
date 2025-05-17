#pragma once
#include <engine/debug/DebugCamera.h>

struct TransformComponent;
struct PlayerComponent;
struct CameraComponent;
class CameraSystem : public System<CameraSystem>
{
public:
	CameraSystem(World* aWorld);
	~CameraSystem() override;

	void Init() override;
	void Update(SceneUpdateContext& dt) override;

	void MouseLean(Camera& camera, const Vector2f& aCameraRotation);
	void SmoothCamera(Camera& camera, const PlayerComponent& aPlayerComponent, TransformComponent& cameraTransform,CameraComponent& aCameraComponent, const SceneUpdateContext& aDeltaTime);
	void WallRunning(Camera& camera, const PlayerComponent& aPlayerComponent, CameraComponent& aCameraComponent);
	void SlideCamera(Camera& camera, const PlayerComponent& aPlayerComponent, CameraComponent& aCameraComponent);
	void RunWiggle();
	void IdleWiggle();
private:
	void HandleLockMouse();
	float myLeanTowards = 0.0f;
	

	//float myHeadBobIntensity = 1.7f;
	//float myHeadBobSpeed = 3.0f;
	float myPlayerVelocity;
	void HandleGodSpeed(const SceneUpdateContext& dt, Camera& camera, const PlayerComponent& aPlayerComponent);
	float myCameraRotationZ = 0;
	float myWantCameraRotationZ = 0;
	float myCameraPositionYOffset = 0;
	bool isLocked = false;
	DebugCamera myDebugCamera;

};