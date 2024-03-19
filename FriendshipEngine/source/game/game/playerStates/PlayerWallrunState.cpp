#include "pch.h"
#include "PlayerWallrunState.h"
#include "PlayerStateMachine.h"
#include "../component/PlayerComponent.h"
#include "../component/TransformComponent.h"
#include <engine/graphics/Animation/AnimationController.h>
#include "../component/PlayerComponent.h"
#include "../component/AnimationDataComponent.h"
#include <physics/PhysXSceneManager.h>

PlayerWallrunState::PlayerWallrunState(PlayerStateMachine* aStateMachine) : PlayerState(aStateMachine)
{
}

void PlayerWallrunState::OnEnter(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;

	float length = p.xVelocity.Length();
	Vector3f wallDirection = p.collision.capsuleNormal.Cross(Vector3f::Up()).GetNormalized();
	Vector3f projectedVector = Vector3f::Project(Vector3f(p.xVelocity.x, 0, p.xVelocity.y), wallDirection);
	projectedVector.Normalize();

	int sign = FriendMath::Sign(projectedVector.Dot(wallDirection));
	p.xVelocity = Vector2f(wallDirection.x, wallDirection.z) * length * (float)sign;

	aContext.animationController->SetParameter(static_cast<size_t>(PlayerAnimationParameter::eWallrunDirection), { -1 }, aContext.animationDataComponent, aContext.animationDataComponent.parameters);
}

void PlayerWallrunState::OnExit(PlayerStateUpdateContext& aContext)
{
	aContext.animationController->SetParameter(static_cast<size_t>(PlayerAnimationParameter::eWallrunDirection), { 0 }, aContext.animationDataComponent, aContext.animationDataComponent.parameters);
}

void PlayerWallrunState::Update(PlayerStateUpdateContext& aContext)
{

	//Update Regular Movement
	aContext;
	PlayerComponent& p = aContext.playerComponent;
	float dt = aContext.deltaTime;
	dt;

	Vector3f wallNormal = p.collision.capsuleNormal;
	if (wallNormal.LengthSqr() == 0)
	{
		wallNormal = p.collision.wallNormal;
	}

	Vector2f inputDirection = p.input.direction;
	Vector3f wallDirection = wallNormal.Cross(Vector3f::Up()).GetNormalized();
	Vector3f projectedVector = Vector3f::Project(Vector3f(p.input.direction.x, 0, p.input.direction.y), wallDirection);
	projectedVector.Normalize();
	float sign = FriendMath::Signf(projectedVector.Dot(wallDirection));
	wallDirection *= sign;

	{
		inputDirection = Vector2f(wallDirection.x, wallDirection.z).GetNormalized();
		p.xVelocity += inputDirection * (PlayerConstants::accelerationSpeed); // Add delta?
	}


	{
		TransformComponent& t = aContext.transformComponent;

		float& camera_pitch = p.cameraRotation.x;
		float& camera_yaw = p.cameraRotation.y;

		// TODO: Restrict camera yaw to 0-90 relative to wall direction here

		camera_yaw = std::fmodf(camera_yaw, 360);
		if (camera_pitch > 90)
			camera_pitch = 90;
		else if (camera_pitch < -90)
			camera_pitch = -90;

		t.transform.SetEulerAngles({ camera_pitch, camera_yaw, 0 });
	}


	float friction = PlayerConstants::wallRunFriction;

	if (p.xVelocity.Length() - friction < 0.01f)
	{
		p.xVelocity = { 0, 0 };
	}
	else// if (!p.isWallRunning)
	{
		Vector2f r = p.xVelocity.GetNormalized() * friction;
		p.xVelocity -= r;
	}

	float gravity = PlayerConstants::gravity * dt;
	if (p.yVelocity + gravity <= 0)
	{
		gravity *= PlayerConstants::wallRunGravityMultiplier;
	}
	p.yVelocity += gravity;

	{
		//SetPlayerState(p, ePlayerClassStates::WallrunState);
		float length = p.xVelocity.Length();
		p.xVelocity = Vector2f(wallDirection.x, wallDirection.z) * length;
	}


	if (p.input.hasPressedJump)
	{
		if (p.isVaultable && (p.input.isHoldingJump || p.input.hasPressedJump))
		{
			//myStateMachine->SetState(ePlayerClassStates::Vault, aContext);
			//return;
		}

		//SetPlayerState(p, ePlayerClassStates::AirbourneState);
		p.yVelocity = PlayerConstants::wallJumpHeight;
		//std::string jumpFilePath = RELATIVE_AUDIO_ASSET_PATH;
		//jumpFilePath += "Jump.wav";
		//AudioManager::GetInstance()->PlaySound(jumpFilePath, { 0,0,0 }, 0.2f);
			// TODO: Restrict wall jump direction outwards from the wall to prevent wall jumping back and forth

		//Vector3f f = Vector3f::Forward() * Matrix3x3f::CreateRotationAroundY(p.cameraRotation.y * Deg2Rad);
		//if (f.Dot(wallNormal) < 0.0f)
		//{
		//	Vector3f wallDirection = wallNormal.Cross(Vector3f::Up()).GetNormalized();
		//	Vector3f projectedVector = Vector3f::Project(f, wallDirection);
		//	projectedVector.Normalize();

		//	float sign = (float)FriendMath::Sign(projectedVector.Dot(wallDirection));

		//	// TODO: Kolla om vi borde enbart cappa till väggens riktning
		//	sign = sign * 2 - 1;
		//	f = Vector3f::Forward() * Matrix3x3f::CreateRotationAroundY((sign * 360.0f - p.cameraRotation.y) * Deg2Rad);
		//}

		//Vector2f forward = { f.x, f.z };
		//float length = p.xVelocity.Length();
		//p.xVelocity = { forward.x * length, forward.y * length };

		//Vector2f wallJumpBoost = Vector2f(wallNormal.x, wallNormal.z);
		//float dot = forward.Dot(wallJumpBoost);
		//float multiplier = 0.0f;
		//if (dot > 0.01f || dot < -0.01f)
		//{
		//	multiplier = p.wallJumpSpeed / (length * dot);
		//}
		//multiplier = std::min(multiplier, 1.0f) * p.wallJumpSpeed;
		//p.xVelocity += wallJumpBoost * multiplier;
		{
			float length = p.xVelocity.Length();
			p.xVelocity = Vector2f(wallNormal.x, wallNormal.z) * PlayerConstants::wallJumpSpeed;
			p.xVelocity += inputDirection * length;
		}



		p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };
		myStateMachine->SetState(ePlayerClassStates::Airborne, aContext);
		return;
	}

	if (p.isGrounded)
	{
		myStateMachine->SetState(ePlayerClassStates::Ground, aContext);
		return;
	}

	if (!CanWallrun(aContext))
	{
		p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };
		myStateMachine->SetState(ePlayerClassStates::Airborne, aContext);
		return;
	}

	p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };
	p.finalVelocity -= wallNormal * 100.0f; // To ensure sticking to the wall

	return;
}

bool PlayerWallrunState::CanWallrun(PlayerStateUpdateContext& aContext)
{
	//PlayerComponent& p = aContext.playerComponent;

	return IsOnWallNextFrame(aContext);

	//Vector3f wallNormal = p.collision.capsuleNormal;
	//if (wallNormal.LengthSqr() == 0)
	//{
	//	wallNormal = p.collision.wallNormal;
	//}

	//if (wallNormal.LengthSqr() == 0) { return false; }
	//return abs(wallNormal.Dot(Vector3f::Up())) < 0.1f && p.xVelocity.Length() > p.minWallRunSpeed;
}

bool PlayerWallrunState::IsOnWallNextFrame(PlayerStateUpdateContext& aContext)
{
	physx::PxRaycastBuffer hit = {};
	auto& p = aContext.playerComponent;
	auto footPos = p.controller->getFootPosition();
	physx::PxTransform shapePose(physx::PxVec3((float)footPos.x, (float)footPos.y + 25.0f, (float)footPos.z));    // [in] initial shape pose (at distance=0)

	Vector3f dir = p.finalVelocity;
	dir -= p.collision.previousCapsuleNormal * (CHARACTER_RADIUS + 10);
	float length = dir.Length();
	dir.Normalize();

	// is wallrunning = false
	auto ppp = p.controller->getPosition();
	if (aContext.physXSceneManager->GetScene()->raycast
	(
		{ (float)ppp.x, (float)ppp.y, (float)ppp.z },
		{ dir.x, dir.y, dir.z },
		length,
		hit,
		physx::PxHitFlag::eDEFAULT,
		physx::PxQueryFilterData(PxQueryFlag::eANY_HIT | physx::PxQueryFlag::eSTATIC)
	))
	{
		p.collision.futureWallNormal = { hit.block.normal.x, hit.block.normal.y, hit.block.normal.z };
		return true;
	}
	return false;

}


