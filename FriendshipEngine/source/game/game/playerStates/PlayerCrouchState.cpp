#include "pch.h"
#include "PlayerCrouchState.h"
#include <physics/PhysXSceneManager.h>
#include "PlayerStateMachine.h"
#include "../component/PlayerComponent.h"

PlayerCrouchState::PlayerCrouchState(PlayerStateMachine* aStateMachine) : PlayerState(aStateMachine)
{
}

void PlayerCrouchState::OnEnter(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;
	p.controller->resize(0.0f);
	p.currentCameraHeight = PlayerConstants::cameraCrouchHeight;
}

void PlayerCrouchState::OnExit(PlayerStateUpdateContext&)
{

}

void PlayerCrouchState::Update(PlayerStateUpdateContext& aContext)
{
	//Update Regular Movement
	aContext;
	PlayerComponent& p = aContext.playerComponent;
	float dt = aContext.deltaTime;
	dt;

	if (!p.isGrounded)
	{
		myStateMachine->SetState(ePlayerClassStates::Airborne, aContext);
		return;
	}

	if (!p.input.isHoldingCrouch && CanUncrouch(aContext))
	{
		myStateMachine->SetState(ePlayerClassStates::Ground, aContext);
		return;
	}

	p.yVelocity = -0.02f;

	if (p.isVaultable && (p.input.isHoldingJump || p.input.hasPressedJump))
	{
		myStateMachine->SetState(ePlayerClassStates::Vault, aContext);
		return;
	}

	if (p.input.hasPressedJump)
	{
		p.yVelocity = PlayerConstants::jumpSpeed;
		myStateMachine->SetState(ePlayerClassStates::Airborne, aContext);
	}

	p.xVelocity += p.input.direction * (PlayerConstants::accelerationSpeed * PlayerConstants::crouchSpeedMultiplier); // *strafeMultiplier; // Add delta? add crouchSpeedMultiplier?

	float friction = PlayerConstants::friction * PlayerConstants::crouchSpeedMultiplier;
	if (p.xVelocity.Length() - friction < 0.01f)
	{
		p.xVelocity = { 0, 0 };
	}
	else
	{
		Vector2f r = p.xVelocity.GetNormalized() * friction;
		p.xVelocity -= r;
	}

	float length = p.xVelocity.Length();
	if (length > PlayerConstants::maxSpeed * PlayerConstants::crouchSpeedMultiplier)
	{
		float delta = length - PlayerConstants::maxSpeed * PlayerConstants::crouchSpeedMultiplier;
		p.xVelocity -= p.xVelocity.GetNormalized() * delta * 0.3f; // vad är det här????
	}

	p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };

	if (p.xVelocity.LengthSqr() > 0)
	{
		//float length = p.xVelocity.Length();
		Vector3f playerVelocity = Vector3f(p.xVelocity.x, p.yVelocity, p.xVelocity.y);
		playerVelocity.y = 0;
		Vector3f moveDirection = playerVelocity.GetNormalized();

		//Vector3f projectedMovement = Vector3f::Cross(Vector3f::Cross(p.collision.groundNormal, Vector3f::Up()), p.collision.groundNormal).GetNormalized() * Vector3f::Dot(moveDirection, p.collision.groundNormal);

		float dist = Vector3f::Dot(playerVelocity, p.collision.groundNormal);
		Vector3f projectedPoint = (playerVelocity)-(p.collision.groundNormal * dist);

		p.finalVelocity = projectedPoint;
	}
}

bool PlayerCrouchState::CanUncrouch(PlayerStateUpdateContext& aContext)
{
	physx::PxOverlapBuffer hit = {};
	auto footPos = aContext.playerComponent.controller->getFootPosition();
	physx::PxTransform shapePose(physx::PxVec3((float)footPos.x, (float)footPos.y + CHARACTER_HEIGHT_PRE_ADJUSTMENT, (float)footPos.z));

	if (aContext.playerComponent.collision.groundNormal.LengthSqr() > 0)
	{
		return !aContext.physXSceneManager->GetScene()->overlap(
			physx::PxSphereGeometry(CHARACTER_RADIUS - 1 + HEAD_BONK_OFFSET),
			shapePose,
			hit,
			physx::PxQueryFilterData(PxQueryFlag::eANY_HIT | physx::PxQueryFlag::eSTATIC)
		);
	}

	return true;
}

