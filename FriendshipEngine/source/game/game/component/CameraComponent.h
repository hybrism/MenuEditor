#pragma once
#include <ecs/component/Component.h>
#include <engine/math/Vector.h>


struct HeadBobData
{
	float myCameraPositionYOffset = 0;
	float myCameraPositionY = 0;


	float headBobSpeedPosY = 7.5f;

	float myCameraRotationX = 0;
	float myCameraRotationY = 0;
	float myCameraRotationZ = 0;

	float velocityIntensity = 0.1f;

	float maxVelocity = 0;

	//Yaw

	float bobSpeedYaw = 7.5f;


	//Pitch

	float PitchBobSpeed = 15.5;

};

struct CameraComponent : public Component<CameraComponent>
{
	float moveSpeed = 400.f;
	float moveMultiplier = 3.f;
	float cameraMoveX = 8.f;
	float cameraMoveY = 5.f;
	bool activeMultiplier = false;
	Vector3f Pos;
	Vector3f Rot;
	HeadBobData headBobData;
};