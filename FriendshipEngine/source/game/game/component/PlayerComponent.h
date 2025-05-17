#pragma once
#include <ecs/component/Component.h>
#include <engine/math/Vector.h>
#include <DirectXMath.h>
#include "../physics/PhysXControllerReportCallbackWrapper.h"
#include "../playerStates/PlayerStatesInclude.h"
#include "../playerStates/PlayerStateMachine.h"
#include "../playerStates/PlayerConstants.h"

namespace physx
{
	class PxController;
}

//enum class ePlayerState
//{
//	AOE,
//	Dash,
//	shield,
//	Ultimate,
//	Dead,
//	RMB,
//	Idle,
//	LMB,
//	Moving,
//	Active,
//	Hurt,
//	Count
//
//};

struct PlayerInputData
{
	Vector2f direction{};
	bool hasPressedJump = false;
	bool isHoldingJump = false;
	bool isHoldingCrouch = false;
};

struct PlayerCollisionData
{
	Vector3f groundNormal;
	Vector3f capsuleNormal;
	Vector3f previousCapsuleNormal;
	Vector3f wallNormal;
	Vector3f futureWallNormal;
	Vector3f capsulePosition;
};

struct PlayerTimers
{
	float godSpeedTimer;
	float slideTimer;
	float coyoteTimer;
	float wallrunTimer;
	float vaultTimer;
};

struct PlayerHealthData
{
	int playerHealth = 2;
	int playerMaxHealth = 2;
};

enum class PlayerAnimationState : size_t
{
	eIdle,
	eRun,
	eSlide,
	eJumpFall,
	eLeftWallRun,
	eRightWallRun,
	eVault
};

enum class PlayerAnimationParameter : size_t
{
	eXSpeed,
	eYSpeed,
	eIsSliding,
	eWallrunDirection
};

struct PlayerComponent : public Component<PlayerComponent>
{
	//Vector3f myRotationSpawn;
	//Vector3f mySpawnPosition;
	//Vector3f targetPosition;
	//Vector3f priorPosition;
  //bool inCutscene; // move somewhere else

	PhysXControllerReportCallbackWrapper callbackWrapper;
	PlayerInputData input;
	PlayerCollisionData collision;
	PlayerHealthData health;
	PlayerTimers timer;


	Vector2f cameraRotation{};
	Vector2f cameraSensitivity = { 8.0f, 5.0f };

	Vector2f xVelocity{};
	float yVelocity = 0.0f;
	float wallRunGravity = 0.0f;
	Vector3f finalVelocity{};

	float currentCameraHeight = PlayerConstants::cameraHeight;
	bool isGrounded = false;
	bool isVaultable = false;
	bool isDead = false;
	bool isChased = false;
	bool isGodSpeed = false;
	bool isVaulting = false;
	bool isPlayerActive;

	Vector3f startVaultPosition;
	Vector3f endVaultPosition;
	float vaultDuration;

	Vector2f savedVaultXVelocity;

	Vector2f wallSlow;

	int isWallRunningDirection = 0;
	float wallTargetLookAngle = 0;
	bool isAdjustWallrunCamera = false;


	Vector3f vaultRayCastOrigin;
	Vector3f vaultLocation{};
	Vector3f SpawnPoint;
	Vector2f SpawnRot;
	Vector2f startGameRot;
	physx::PxController* controller = nullptr;
	ePlayerClassStates currentPlayerState;
};