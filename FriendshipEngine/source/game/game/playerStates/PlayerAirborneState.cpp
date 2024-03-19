#include "pch.h"
#include "PlayerAirborneState.h"
#include "PlayerStateMachine.h"
#include "../component/PlayerComponent.h"
#include <physics/PhysXSceneManager.h>

PlayerAirborneState::PlayerAirborneState(PlayerStateMachine* aStateMachine) : PlayerState(aStateMachine)
{
}

void PlayerAirborneState::OnEnter(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;
	p.currentCameraHeight = PlayerConstants::cameraHeight;
	p.controller->resize(CHARACTER_HEIGHT);
}

void PlayerAirborneState::OnExit(PlayerStateUpdateContext&)
{
}

void PlayerAirborneState::Update(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;
	float dt = aContext.deltaTime;

	// IBLAND FASTNAR MAN FÖR MAN INTE ÄR GROUNDED NÄR MAN BORDE VARA DET

	float gravity = PlayerConstants::gravity * dt;


	//if (p.collision.capsuleNormal.LengthSqr() > 0 && abs(p.collision.capsuleNormal.Dot(Vector3f::Up())) < 0.1f)
	//{
	//	Vector2f norm = { p.collision.capsuleNormal.x, p.collision.capsuleNormal.z };
	//	float dot = p.input.direction.Dot(norm);
	//	if (dot > 0.85f)
	//	{
	//		p.isWallRunning = false;
	//	}
	//}
	//else
	//{
	//	p.isWallRunning = false;
	//}

	if (p.collision.groundNormal.Dot(Vector3f::Up()) < -0.5f && p.yVelocity > 0)
	{
		p.yVelocity = 0;
	}

	p.yVelocity += gravity;

	float strafeMultiplier = 1.0f;
	if (p.xVelocity.LengthSqr() > 0)
	{
		strafeMultiplier = std::max(0.2f, 1.0f - p.input.direction.Dot(p.xVelocity.GetNormalized()));
	}
	p.xVelocity += p.input.direction * (PlayerConstants::accelerationSpeed)*strafeMultiplier * PlayerConstants::airMobilityMultiplier; // Add delta?


	float friction = PlayerConstants::airFriction;

	if (p.xVelocity.Length() - friction < 0.01f)
	{
		p.xVelocity = { 0, 0 };
	}
	else
	{
		Vector2f r = p.xVelocity.GetNormalized() * friction;
		p.xVelocity -= r;
	}

	if (abs(p.collision.wallNormal.Dot(Vector3f::Up()) < 0.1f) && p.collision.wallNormal.LengthSqr() != 0)
	{
		if (!IsLookingAtWallForMagnetEffect(aContext))
		{
			Vector2f magnetMoveAmount = { p.collision.wallNormal.x, p.collision.wallNormal.z };
			magnetMoveAmount *= -PlayerConstants::wallRunWallDetectionMagnetSpeed;
			p.xVelocity += magnetMoveAmount;
			PrintI("Is using Magnet Power.");
		}
	}

	p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };

	if (p.isGrounded)
	{
		if (p.input.isHoldingCrouch && p.xVelocity.Length() >= PlayerConstants::slideSpeedThreshold)
		{
			myStateMachine->SetState(ePlayerClassStates::Slide, aContext);
			return;
		}

		myStateMachine->SetState(ePlayerClassStates::Ground, aContext);
		return;
	}

	if (CanWallrun(aContext))
	{
		myStateMachine->SetState(ePlayerClassStates::Wallrun, aContext);
		return;
	}

	if (p.isVaultable && (p.input.isHoldingJump || p.input.hasPressedJump))
	{
		myStateMachine->SetState(ePlayerClassStates::Vault, aContext);
		return;
	}
}

bool PlayerAirborneState::CanWallrun(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;
	if (p.collision.capsuleNormal.LengthSqr() == 0) { return false; }


	return (abs(p.collision.capsuleNormal.Dot(Vector3f::Up())) < 0.1f && p.xVelocity.Length() > PlayerConstants::minWallRunSpeed && (!IsLookingAtWall(aContext)));

	//return abs(p.collision.capsuleNormal.Dot(Vector3f::Up())) < 0.1f && p.xVelocity.Length() > PlayerConstants::minWallRunSpeed;
}

bool PlayerAirborneState::IsLookingAtWall(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;

	Matrix4x4f rot = Matrix4x4f::CreateRollPitchYawMatrix(0, p.cameraRotation.y, 0);
	Vector3f forward = { rot(3, 1), rot(3, 2), rot(3, 3) };
	float dot = /*abs*/(-p.collision.capsuleNormal.Dot(forward));
	//PrintI(std::to_string(dot));
	//Check Picture
	//https://docs.unity3d.com/uploads/Main/CosineValues.png

	if (!(-0.86f < dot && dot < 0.87f))
	{
		//PrintI("Is looking at wall.");
		return true;
	}
	return false;
}

bool PlayerAirborneState::IsLookingAtWallForMagnetEffect(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;

	Matrix4x4f rot = Matrix4x4f::CreateRollPitchYawMatrix(0, p.cameraRotation.y, 0);
	Vector3f forward = { rot(3, 1), rot(3, 2), rot(3, 3) };
	float dot = /*abs*/(-p.collision.wallNormal.Dot(forward));
	//PrintI(std::to_string(dot));
	//Check Picture
	//https://docs.unity3d.com/uploads/Main/CosineValues.png

	if (!(-0.86f < dot && dot < 0.87f))
	{
		//PrintI("Is looking at wall.");
		return true;
	}


	return false;
}
