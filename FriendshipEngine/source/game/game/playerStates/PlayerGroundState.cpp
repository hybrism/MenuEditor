#include "pch.h"
#include "PlayerGroundState.h"
#include "PlayerStateMachine.h"
#include "../component/PlayerComponent.h"
#include <physics/PhysXSceneManager.h>
#include <engine/graphics/Animation/AnimationController.h>
#include "../component/AnimationDataComponent.h"
#include "audio/NewAudioManager.h"
#include <iostream>

PlayerGroundState::PlayerGroundState(PlayerStateMachine* aStateMachine) : PlayerState(aStateMachine)
{
}

void PlayerGroundState::OnEnter(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;
	p.currentCameraHeight = PlayerConstants::cameraHeight;
	p.controller->resize(CHARACTER_HEIGHT);
	//p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };
}

void PlayerGroundState::OnExit(PlayerStateUpdateContext& aContext)
{
	aContext.animationDataComponent.speed = 1.0f;
}

void PlayerGroundState::Update(PlayerStateUpdateContext& aContext)
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

	if (p.input.isHoldingCrouch)
	{
		if (p.xVelocity.Length() >= PlayerConstants::slideSpeedThreshold)
		{
			myStateMachine->SetState(ePlayerClassStates::Slide, aContext);
			return;
		}

		myStateMachine->SetState(ePlayerClassStates::Crouch, aContext);
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
		p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };
		myStateMachine->SetState(ePlayerClassStates::Airborne, aContext);
		return;
	}

	p.xVelocity += p.input.direction * (PlayerConstants::accelerationSpeed); // *strafeMultiplier; // Add delta?

	//if (p.collision.capsuleNormal.LengthSqr() > 0)
	//{
	//	// Is wallrunning = true eller om man st?r vid en v?gg
	//	Vector3f vel = { p.xVelocity.x, 0, p.xVelocity.y };
	//	float dot = vel.GetNormalized().Dot(p.collision.capsuleNormal * -1.0f);
	//	if (dot > 0.1f)
	//	{
	//		Vector2f norm = { p.collision.capsuleNormal.x, p.collision.capsuleNormal.z };
	//		norm *= p.xVelocity.Length() * dot;
	//		p.xVelocity += norm;
	//	}
	//}

	// On high frame rates, you stop INSTANTLY, also making the slide SUCK D:
	float friction = PlayerConstants::friction;
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
	if (length > PlayerConstants::groundMaxSpeed)
	{
		//float delta = length - p.groundMaxSpeed;
		//delta = std::min(delta, p.groundedMaxSpeedCorrectionAmount);
		//p.xVelocity -= p.xVelocity.GetNormalized() * delta;
		p.xVelocity.Normalize();
		p.xVelocity *= PlayerConstants::groundMaxSpeed;
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

	if (aContext.animationDataComponent.currentStateIndex == (int)PlayerAnimationState::eRun)
	{
		aContext.animationDataComponent.speed = std::min(std::max(p.xVelocity.Length(), 0.2f), PlayerConstants::groundMaxSpeed) / PlayerConstants::groundMaxSpeed;
	}
}


