#include "pch.h"
#include <physics/PhysXSceneManager.h>
#include "PlayerSlideState.h"
#include "PlayerStateMachine.h"
#include <engine/graphics/Animation/AnimationController.h>
#include "../component/PlayerComponent.h"
#include "../component/AnimationDataComponent.h"

PlayerSlideState::PlayerSlideState(PlayerStateMachine* aStateMachine) : PlayerState(aStateMachine)
{
}

void PlayerSlideState::OnEnter(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;
	p.xVelocity += p.xVelocity.GetNormalized() * PlayerConstants::slideSpeedBoost;
	p.controller->resize(0.0f);
	p.currentCameraHeight = PlayerConstants::cameraCrouchHeight;

	aContext.animationController->SetParameter(static_cast<size_t>(PlayerAnimationParameter::eIsSliding), { true }, aContext.animationDataComponent, aContext.animationDataComponent.parameters);
}

void PlayerSlideState::OnExit(PlayerStateUpdateContext& aContext)
{
	aContext.animationController->SetParameter(static_cast<size_t>(PlayerAnimationParameter::eIsSliding), { false }, aContext.animationDataComponent, aContext.animationDataComponent.parameters);

}

// TODO: SLIDE DIRECTION INFLUENCE
void PlayerSlideState::Update(PlayerStateUpdateContext& aContext)
{
	//Update Regular Movement
	aContext;
	PlayerComponent& p = aContext.playerComponent;
	float dt = aContext.deltaTime;
	dt;
	p;

	if (!p.isGrounded)
	{
		myStateMachine->SetState(ePlayerClassStates::Airborne, aContext);
		return;
	}

	bool canUncrouch = CanUncrouch(aContext);
	float friction = PlayerConstants::slideFriction;
	if (p.xVelocity.Length() - friction < 0.01f)
	{
		p.xVelocity = { 0, 0 };
	}
	else
	{
		Vector2f r = p.xVelocity.GetNormalized() * friction;
		p.xVelocity -= r;
	}

	if (p.input.hasPressedJump && canUncrouch)
	{
		p.yVelocity = PlayerConstants::jumpSpeed;
		p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };

		myStateMachine->SetState(ePlayerClassStates::Airborne, aContext);
		return;
	}

	p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };

	if (canUncrouch &&
		!p.input.isHoldingCrouch &&
		p.xVelocity.Length() < PlayerConstants::slideSpeedThreshold
	)
	{
		myStateMachine->SetState(ePlayerClassStates::Ground, aContext);
		return;
	}

	if (p.xVelocity.Length() < PlayerConstants::slideSpeedThreshold)
	{
		myStateMachine->SetState(ePlayerClassStates::Crouch, aContext);
		return;
	}
}

bool PlayerSlideState::CanUncrouch(PlayerStateUpdateContext& aContext)
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


