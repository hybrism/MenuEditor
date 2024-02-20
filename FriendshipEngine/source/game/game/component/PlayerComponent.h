#pragma once
#include <ecs/component/Component.h>
#include <engine/math/Vector.h>
#include <DirectXMath.h>
#include "../physics/PhysXControllerReportCallbackWrapper.h"

namespace physx
{
	class PxController;
}

enum class ePlayerState
{
	AOE,
	Dash,
	shield,
	Ultimate,
	Dead,
	RMB,
	Idle,
	LMB,
	Moving,
	Active,
	Hurt,
	Count

};

struct PlayerComponent : public Component<PlayerComponent>
{
	//Vector3f myRotationSpawn;
	//Vector3f mySpawnPosition;
	//Vector3f targetPosition;
	//Vector3f priorPosition;
//	bool inCutscene; // move somewhere else

	PhysXControllerReportCallbackWrapper callbackWrapper;
	Vector2f xVelocity{};
	float yVelocity = 0.0f;
	Vector2f inputDirection{};
	Vector3f groundCollisionNormal{};
	Vector3f capsuleCollisionNormal{};
	Vector3f wallNormal{};
	Vector3f capsuleCollisionPosition{};
	Vector2f cameraRotation{};
	Vector2f cameraSensitivity = { 8.0f, 5.0f };
	Vector3f finalVelocity{};
	float maxSpeed = 11.6f; // (x * 6) / 10 = m/s, 10 * m/s / 6 = cm/frame,			7.25, 8.7, 11.6
	//float maxSpeed = 11.6f; // (x * 6) / 10 = m/s, 10 * m/s / 6 = cm/frame
	float speed = maxSpeed * 1.5f / 6.0f;
	float wallRunRaycastLength = 100.0f;
	float gravity = -45.0f;
	float jumpSpeed = 15.0f;
	float friction = speed * 0.5f;
	float airFriction = 0.1f;
	float cameraHeight = 160.0f/* + 40.0f*/;
	float cameraCrouchHeight = 90.0f/* + 40.0f*/;
	float wallJumpSpeed = 20.0f;
	float airMobilityMultiplier = 0.2f;
	float wallRunGravityMultiplier = 0.2f;
	float vaultRange = 35.0f;
	float crouchSpeedMultiplier = 0.75f;
	//float vaultHeight = 50.0f;
	float slideSpeed = 1.1f;
	float slideFriction = 0.95f;
	float slideSpeedThreshold = maxSpeed / 2.0f;
	float slideTimer = 0.0f;
	float slideTimeUnitlFriction = 0.5f;
	bool hasPressedJump = false;
	bool isHoldingCrouch = false;
	bool isCrouching = false;
	bool isSliding = false;
	bool isGrounded = false;
	bool isWallRunning = false;
	bool isVaultable = false;
	Vector3f vaultLocation{};

	physx::PxController* controller = nullptr;
	bool isPlayerActive;
	ePlayerState playerState = ePlayerState::Idle;
};