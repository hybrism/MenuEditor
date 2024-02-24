#pragma once
#include "../graphics/Camera.h"

struct DebugCameraData
{
	float myMoveSpeed = 400.0f;
	float myMoveMultiplier = 3.0f;
	float myCameraMoveX = 8.0f;
	float myCameraMoveY = 5.0f;
};

class DebugCamera
{
public:
	DebugCamera(const DebugCameraData& aData = {});
	~DebugCamera();

	void Update(float aDeltaTime);
	Camera myCamera;
private:
	float Multiplier() const;

	DebugCameraData myData;
	float myYaw;
	float myPitch;
	bool myActiveMultiplier;
};