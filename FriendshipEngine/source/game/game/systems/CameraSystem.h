#pragma once

class CameraSystem : public System
{
public:
	CameraSystem(World* aWorld);
	~CameraSystem() override;

	void Init() override;
	void Update(const float& dt) override;
private:
	void HandleCameraMovement(const float& dt, const Entity& aEntity );
	void HandleLockMouse();
	bool isLocked = false;

};