#include "pch.h"
#include "PlayerAirborneState.h"
#include "PlayerStateMachine.h"
#include "../component/PlayerComponent.h"
#include <physics/PhysXSceneManager.h>
#include <audio/NewAudioManager.h>

PlayerAirborneState::PlayerAirborneState(PlayerStateMachine* aStateMachine) : PlayerState(aStateMachine)
{
}

void PlayerAirborneState::OnEnter(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;
	p.currentCameraHeight = PlayerConstants::cameraHeight;
	p.controller->resize(CHARACTER_HEIGHT);
	p.timer.coyoteTimer = PlayerConstants::coyoteJumpMaxTime;
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

	if (std::isnan(p.xVelocity.x))
	{
 		PrintI("X IS NAN");
		p.xVelocity.x = 0;
	}
	if (std::isnan(p.xVelocity.y))
	{
		PrintI("Y IS NAN");
		p.xVelocity.y = 0;
	}
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

	p.timer.coyoteTimer -= dt;

	bool coyoteJumpEnable = p.timer.coyoteTimer > 0 ? true : false;

	if (coyoteJumpEnable)
	{
		if (p.input.hasPressedJump && p.yVelocity < 0)
		{
			p.yVelocity = PlayerConstants::jumpSpeed;
			PrintI("Coyoteeeee!");
		}
	}
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

	Vector2f finalVelocityHorizontal = { p.xVelocity.x, p.xVelocity.y };
	//PrintI(std::to_string(p.xVelocity.Length()));

	if (p.xVelocity.Length() >= PlayerConstants::airMaxSpeed)
	{
		p.xVelocity.Normalize();
		p.xVelocity *= PlayerConstants::airMaxSpeed;
		//PrintI("CAP");
	}
	p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };

	if (p.isGrounded)
	{
		if (p.input.isHoldingCrouch && p.xVelocity.Length() >= PlayerConstants::slideSpeedThreshold)
		{
			myStateMachine->SetState(ePlayerClassStates::Slide, aContext);
			return;
		}

		NewAudioManager::GetInstance()->PlaySound(eSounds::Land, 0.15f, 0.92f);
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
	if (std::isnan(p.finalVelocity.x))
		PrintI("X IS NAN");
	if (std::isnan(p.finalVelocity.y))
		PrintI("Y IS NAN");
	if (std::isnan(p.finalVelocity.z))
		PrintI("Z IS NAN");
}

bool PlayerAirborneState::CanWallrun(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;
	if (p.collision.capsuleNormal.LengthSqr() == 0) { return false; }
	Matrix4x4f rot = Matrix4x4f::CreateRollPitchYawMatrix(0, p.cameraRotation.y, 0);
	Vector3f forward = { rot(3, 1), rot(3, 2), rot(3, 3) };
	Vector3f normalizedVelocity = { p.xVelocity.x, 0, p.xVelocity.y };
	normalizedVelocity.Normalize();
	float dotLooking = normalizedVelocity.Dot(forward);

	//int lookingSign = FriendMath::Sign(dotLooking);
	if (dotLooking < -0.3f)
	{
		PrintI("Not Looking same dir moving");
		return false;
	}

#pragma region TODO, Fix so we cant move sideways to wall to enter Wallrun
	////PrintI("Looking Sign: " + std::to_string(lookingSign));

	//Vector3f wallDirection = p.collision.capsuleNormal.Cross(Vector3f::Up()).GetNormalized();
	////Vector3f projectedLookVector = Vector3f::Project(forward, p.collision.capsuleNormal);
	////projectedLookVector.Normalize();

	////float dot = p.collision.capsuleNormal.Dot(projectedLookVector);
	////PrintI("Dot: " + std::to_string(dot));

	////int sign = FriendMath::Sign(dot);
	////PrintI("Wall Z: " + std::to_string(projectedLookVector.x) + " | " + std::to_string(projectedLookVector.y) + " | " + std::to_string(projectedLookVector.z));


	//Vector3f projectedVector = Vector3f::Project(Vector3f(p.xVelocity.x, 0, p.xVelocity.y), wallDirection);
	//projectedVector.Normalize();
	////float dotVelocityVsWall = p.collision.capsuleNormal.Dot(projectedVector);
	//float dotVelocityVsWall = projectedVector.Dot(normalizedVelocity);
	//PrintI("Velocity Dot: " + std::to_string(dotVelocityVsWall));

	////PrintI("Sign: " + std::to_string(sign));


	//PrintI("Projected Vector: " + std::to_string(projectedVector.x) + " | " + std::to_string(projectedVector.y) + " | " + std::to_string(projectedVector.z));
	////PrintI("Wall Z: " + std::to_string(p.collision.capsuleNormal.x) + " | " + std::to_string(p.collision.capsuleNormal.y) + " | " + std::to_string(p.collision.capsuleNormal.z));
	//PrintI("Wall Z: " + std::to_string(normalizedVelocity.x) + " | " + std::to_string(normalizedVelocity.y) + " | " + std::to_string(normalizedVelocity.z));
	////if (dotVelocityVsWall < 0)
	////{
	////	return false;
	////}
#pragma endregion


	return (abs(p.collision.capsuleNormal.Dot(Vector3f::Up())) < 0.1f && p.xVelocity.Length() > PlayerConstants::minWallRunSpeed && (!IsLookingAtWall(aContext)));

	//return abs(p.collision.capsuleNormal.Dot(Vector3f::Up())) < 0.1f && p.xVelocity.Length() > PlayerConstants::minWallRunSpeed;
}

bool PlayerAirborneState::IsLookingAtWall(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;

	Matrix4x4f rot = Matrix4x4f::CreateRollPitchYawMatrix(0, p.cameraRotation.y, 0);
	Vector3f forward = { rot(3, 1), rot(3, 2), rot(3, 3) };
	float dot = /*abs*/(-p.collision.capsuleNormal.Dot(forward));
	PrintI(std::to_string(dot));
	//Check Picture
	//https://docs.unity3d.com/uploads/Main/CosineValues.png

	//return !(-0.86f < dot && dot < 0.87f);
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
