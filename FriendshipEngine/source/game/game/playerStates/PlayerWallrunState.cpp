#include "pch.h"
#include "PlayerWallrunState.h"
#include "PlayerStateMachine.h"
#include "../component/PlayerComponent.h"
#include "../component/TransformComponent.h"
#include <engine/graphics/Animation/AnimationController.h>
#include "../component/PlayerComponent.h"
#include "../component/AnimationDataComponent.h"
#include <physics/PhysXSceneManager.h>
#include "audio/NewAudioManager.h"
#include <random>
#include "audio/NewAudioManager.h"

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
	p.isWallRunningDirection = sign;

	aContext.animationController.SetParameter(static_cast<size_t>(PlayerAnimationParameter::eWallrunDirection), { -sign }, aContext.animationDataComponent, aContext.animationDataComponent.parameters);

	p.timer.wallrunTimer = 0.f;
	if (p.yVelocity > 0)
		p.yVelocity /= 2.f;
	else
		p.yVelocity /= 4.f;



	Vector3f wallNormal = p.collision.capsuleNormal;

	Vector3f playerDirection = { p.xVelocity.x, 0, p.xVelocity.y };
	playerDirection.Normalize();

	// Camera Lock
	float wallAngle = atan2(wallNormal.x, wallNormal.z) * Rad2Deg;
	bool isWallOnLeft = playerDirection.Cross(wallNormal).y > 0.f;

	float limitAwayFromWall = wallAngle + 20.f * -((int)isWallOnLeft * 2 - 1);
	float limitTowardsWall = wallAngle + 100.f * -((int)isWallOnLeft * 2 - 1);

	FriendMath::ClampAngle(limitTowardsWall);
	FriendMath::ClampAngle(limitAwayFromWall);

	float& camera_yaw = p.cameraRotation.y;
	{
		if (sign < 0)
		{
			std::swap(limitTowardsWall, limitAwayFromWall);
		}

		if (!IsKnifeInPieSlice(camera_yaw, limitTowardsWall, limitAwayFromWall))
		{
			PrintI("On Wallrun Enter");

			p.isAdjustWallrunCamera = true;
			p.timer.vaultTimer = 0;

			if (sign > 0)
				p.wallTargetLookAngle = limitTowardsWall;
			else
				p.wallTargetLookAngle = limitAwayFromWall;


	
	//p.timer.wallrunTimer = 0.0f;
	//p.wallRunGravity = 0.0f;
	NewAudioManager::GetInstance()->PlayLoopingSound(eDedicatedChannels::WallRun, eSounds::WallRun, 1.5f);
		}
	}
}

void PlayerWallrunState::OnExit(PlayerStateUpdateContext& aContext)
{
	aContext.animationController.SetParameter(static_cast<size_t>(PlayerAnimationParameter::eWallrunDirection), { 0 }, aContext.animationDataComponent, aContext.animationDataComponent.parameters);

	NewAudioManager::GetInstance()->StopLoopingSound(eDedicatedChannels::WallRun);
}

void PlayerWallrunState::Update(PlayerStateUpdateContext& aContext)
{

	//Update Regular Movement
	aContext;
	PlayerComponent& p = aContext.playerComponent;
	float dt = aContext.deltaTime;

	Vector3f wallNormal = p.collision.capsuleNormal;
	if (wallNormal.LengthSqr() == 0)
	{
		wallNormal = p.collision.wallNormal;
	}
	float sign = 0;
	Vector2f inputDirection = p.input.direction;
	Vector3f wallDirection = wallNormal.Cross(Vector3f::Up()).GetNormalized();
	sign = (float)p.isWallRunningDirection;
	wallDirection *= sign;

	if (inputDirection.x == 0 && inputDirection.y == 0 || p.isAdjustWallrunCamera)
	{
		//wallDirection *= (float)p.isWallRunningDirection;
		inputDirection = Vector2f(wallDirection.x, wallDirection.z).GetNormalized();
		p.xVelocity += inputDirection * (PlayerConstants::accelerationSpeed) * 0.9f; // Add delta?
	}
	else
	{
		if (wallDirection.x < 0)
			inputDirection.x = std::clamp(inputDirection.x, wallDirection.x, 0.f);
		else
			inputDirection.x = std::clamp(inputDirection.x, 0.f, wallDirection.x);

		if (wallDirection.z < 0)
			inputDirection.y = std::clamp(inputDirection.y, wallDirection.z, 0.f);
		else
			inputDirection.y = std::clamp(inputDirection.y, 0.f, wallDirection.z);
		

		//PrintI("Input: " + std::to_string(inputDirection.x) + " | "  + std::to_string(inputDirection.y))
		Vector3f projectedVector = Vector3f::Project(Vector3f(inputDirection.x, 0, inputDirection.y), wallDirection);
		projectedVector.Normalize();
		//sign = (float)p.isWallRunningDirection;
		//wallDirection *= sign;
		inputDirection = Vector2f(projectedVector.x, projectedVector.z)/*.GetNormalized()*/;
		p.xVelocity += inputDirection * (PlayerConstants::accelerationSpeed); // Add delta?
	}

	{
		TransformComponent& t = aContext.transformComponent;

		if (p.isAdjustWallrunCamera)
		{
			AdjustWallrunCameraOnEnter(aContext, p, t);
		}
		else
		{
			LimitWallrunCamera(p, t, sign);
		}

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


	//Damp Gravity / Phase 1

	if (p.yVelocity > 0)
	{
		float gravity = PlayerConstants::gravity * dt;
		p.yVelocity += gravity;
	}
	else
	{
		if (p.timer.wallrunTimer < 2.1f)
		{
			p.timer.wallrunTimer += dt;
		}

		float gravity = powf(5.f, p.timer.wallrunTimer) * -1.f;
		p.yVelocity += gravity * dt;
	}

	//if (p.yVelocity <= 0)
	//{
	//	float gravity = PlayerConstants::gravity * dt;

	//	auto result = PlayerConstants::wallRunFallDamping * dt * (1 - std::min(p.timer.wallrunTimer / 1.0f, 1.0f));
	//	PrintI("Gravity: " + std::to_string(result));
	//	if (p.yVelocity + result >= gravity * PlayerConstants::wallRunGravityMultiplier)
	//	{
	//		p.timer.wallrunTimer += dt;
	//	}

	//	p.yVelocity = std::max(0.0f, p.yVelocity + result);
	//}


	////EXPO Gravity / Phase 2
	//if (p.timer.wallrunTimer > 0.001f)
	//{
	//	p.timer.wallrunTimer += dt;

	//	float gravity = PlayerConstants::gravity;

	//	p.wallRunGravity += gravity * dt;
	//	p.yVelocity += p.wallRunGravity * dt;
	//	//float gravity = powf(5.f, p.timer.wallrunTimer) * -1.f;
	//	//if (p.yVelocity + gravity <= 0)
	//	//{
	//	//	gravity *= PlayerConstants::wallRunGravityMultiplier;
	//	//}
	//}



	{
		//SetPlayerState(p, ePlayerClassStates::WallrunState);
		float length = p.xVelocity.Length();
		p.xVelocity = Vector2f(wallDirection.x, wallDirection.z) * length;
	}

#pragma region New Wall Detection
//	if ( !(p.collision.previousWallNormal.x == 0 && p.collision.previousWallNormal.y == 0 && p.collision.previousWallNormal.z == 0) &&  
//		!(p.collision.wallNormal.x == 0 && p.collision.wallNormal.y == 0 &&	p.collision.wallNormal.z == 0) 
//		/*&&	p.isAdjustWallrunCamera == false*/)
//	{
///*		if (abs(p.collision.previousCapsuleNormal.x - p.collision.capsuleNormal.x) > 0.1f ||
//			abs(p.collision.previousCapsuleNormal.y - p.collision.capsuleNormal.y) > 0.1f ||
//			abs(p.collision.previousCapsuleNormal.z - p.collision.capsuleNormal.z) > 0.1f)	*/	
//		if (Vector3f::Dot(p.collision.previousWallNormal, p.collision.wallNormal) < 0.8f)
//		{
//			PrintI("New Wall");
//			PrintI("WallNormal: " + std::to_string(p.collision.wallNormal.x) + " | " + std::to_string(p.collision.wallNormal.y) + " | " + std::to_string(p.collision.wallNormal.z));
//			PrintI("Previous: " + std::to_string(p.collision.previousWallNormal.x) + " | " + std::to_string(p.collision.previousWallNormal.y) + " | " + std::to_string(p.collision.previousWallNormal.z));
//			p.isAdjustWallrunCamera = true;
//			//float length = p.xVelocity.Length();
//			//Vector3f newWallDirection = p.collision.capsuleNormal.Cross(Vector3f::Up()).GetNormalized();
//			//Vector3f newProjectedVector = Vector3f::Project(Vector3f(p.xVelocity.x, 0, p.xVelocity.y), newWallDirection);
//			//newProjectedVector.Normalize();
//			//Vector3f newWallNormal = p.collision.capsuleNormal;
//
//			//Vector3f newPlayerDirection = { p.xVelocity.x, 0, p.xVelocity.y };
//			//newPlayerDirection.Normalize();
//
//			//// Camera Lock
//			//float wallAngle = atan2(newWallNormal.x, newWallNormal.z) * Rad2Deg;
//			//bool isWallOnLeft = newPlayerDirection.Cross(newWallNormal).y > 0.f;
//
//			//float limitAwayFromWall = wallAngle + 20.f * -((int)isWallOnLeft * 2 - 1);
//			//float limitTowardsWall = wallAngle + 100.f * -((int)isWallOnLeft * 2 - 1);
//
//			//FriendMath::ClampAngle(limitTowardsWall);
//			//FriendMath::ClampAngle(limitAwayFromWall);
//			//float& camera_yaw = p.cameraRotation.y;
//			//{
//			//	if (sign < 0)
//			//	{
//			//		std::swap(limitTowardsWall, limitAwayFromWall);
//			//	}
//
//			//	if (!IsKnifeInPieSlice(camera_yaw, limitTowardsWall, limitAwayFromWall))
//			//	{
//			//		PrintI("On Wallrun Enter");
//
//			//		p.isAdjustWallrunCamera = true;
//			//		p.timer.vaultTimer = 0;
//
//			//		if (sign > 0)
//			//			p.wallTargetLookAngle = limitTowardsWall;
//			//		else
//			//			p.wallTargetLookAngle = limitAwayFromWall;
//
//
//			//	}
//			//}
//
//			//int newSign = FriendMath::Sign(newProjectedVector.Dot(newWallDirection));
//			//p.xVelocity = Vector2f(newWallDirection.x, newWallDirection.z) * length * (float)newSign;
//			//p.isWallRunningDirection = newSign;
//		}
//	}
#pragma endregion

	if (p.xVelocity.Length() >= PlayerConstants::airMaxSpeed)
	{
		p.xVelocity.Normalize();
		p.xVelocity *= PlayerConstants::airMaxSpeed;
		//PrintI("CAP");
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

#pragma region Legacy Code
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

		//	// TODO: Kolla om vi borde enbart cappa till v?ggens riktning
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
#pragma endregion

		{
			float length = p.xVelocity.Length();
			Matrix4x4f rot = Matrix4x4f::CreateRollPitchYawMatrix(0, p.cameraRotation.y, 0);
			Vector2f playerLookDirection = { rot(3, 1), rot(3, 3) };
			Vector2f wallOutSpeed = Vector2f(wallNormal.x, wallNormal.z) * PlayerConstants::wallJumpOutSpeed;
			Vector2f wallLookJumpSpeed = playerLookDirection * PlayerConstants::wallJumpSpeed;
			//p.xVelocity = Vector2f(wallNormal.x, wallNormal.z) * PlayerConstants::wallJumpSpeed;
			p.xVelocity = wallLookJumpSpeed + wallOutSpeed;
			p.xVelocity += inputDirection * length;
		}


		p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };
		myStateMachine->SetState(ePlayerClassStates::Airborne, aContext);
		return;
	}

	if (p.isGrounded)
	{
		p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };
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
	//p.finalVelocity -= wallNormal * 100.0f; // To ensure sticking to the wall
	p.finalVelocity -= wallNormal * 100.0f; // To ensure sticking to the wall


	static float timermax = .20f;
	static float timercounter = 0;

	if (p.currentPlayerState == ePlayerClassStates::Wallrun)
	{
		timercounter += aContext.deltaTime;
		if (timercounter > timermax)
		{
	/*		std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int> dis(10, 30);

			int randomNumber = dis(gen);*/

			timercounter = 0;
			NewAudioManager::GetInstance()->PlaySound(eSounds::Run, 0.2f, 1.0f);
		}
	}
	//PrintI("Wallnormal: " + std::to_string(wallNormal.x) + " | " + std::to_string(wallNormal.y) + " | " + std::to_string(wallNormal.z));
	return;
}

bool PlayerWallrunState::CanWallrun(PlayerStateUpdateContext& aContext)
{
	return IsOnWallNextFrame(aContext);
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
		//float dotProduct = p.collision.futureWallNormal.Dot(p.collision.previousCapsuleNormal);
		//float dotProduct = p.collision.futureWallNormal.Dot(p.collision.capsuleNormal);
		//if (dotProduct < 0.9f)
		//{
		//	if (p.collision.capsuleNormal.x == 0 && p.collision.capsuleNormal.y == 0 && p.collision.capsuleNormal.z == 0)
		//	{
		//		PrintI("FUTURE NORMAL but capsule is 0");
		//		return true;
		//	}
		//	PrintI("PreviousWall VS Future Dot:" + std::to_string(dotProduct));
		//	p.isAdjustWallrunCamera = true;
		// 
		// 
		//  INSTEAD OF TRYING TO MAKE IT LOOK AT NEXT WALL AND CONTINUE WALLRUN WE MIGHT JUST SET THE PLAYER TO AIRBOURNCE STATE WITH 0 velocity
		// 
		// 
		//}
		return true;
	}
	return false;

}

bool PlayerWallrunState::IsKnifeInPieSlice(float aAngle, float aLowerBound, float aUpperBound)
{
	if (aLowerBound < aUpperBound)
	{
		return (aAngle >= aLowerBound && aAngle <= aUpperBound);
	}
	else
	{
		return (aAngle >= aLowerBound || aAngle <= aUpperBound);
	}
}

void PlayerWallrunState::AdjustWallrunCameraOnEnter(PlayerStateUpdateContext& aContext, PlayerComponent& p, TransformComponent& t)
{
	float& camera_yaw = p.cameraRotation.y;

	p.timer.vaultTimer = std::min(0.3f, p.timer.vaultTimer + aContext.deltaTime);

	camera_yaw = FriendMath::Lerp(camera_yaw, p.wallTargetLookAngle, p.timer.vaultTimer / 0.3f);
	if (p.timer.vaultTimer >= 0.3f || abs(camera_yaw - p.wallTargetLookAngle) < 3.f)
	{
		camera_yaw = p.wallTargetLookAngle;
		p.isAdjustWallrunCamera = false;
		p.timer.vaultTimer = 0;
	}
	float& camera_pitch = p.cameraRotation.x;
	if (camera_pitch > 90)
		camera_pitch = 90;
	else if (camera_pitch < -90)
		camera_pitch = -90;

	t.transform.SetEulerAngles({ camera_pitch, camera_yaw, 0 });
}

void PlayerWallrunState::LimitWallrunCamera(PlayerComponent& p, TransformComponent& t, const float& aSign)
{
	Matrix4x4f rot = Matrix4x4f::CreateRollPitchYawMatrix(0, p.cameraRotation.y, 0);
	Vector3f wallNormal = p.collision.capsuleNormal;
	if (wallNormal.x == 0 && wallNormal.y == 0 && wallNormal.z == 0)
	{
		PrintI("WALL NORMAL ZERO!");
		return;
	}
	Vector3f playerDirection = { p.xVelocity.x, 0, p.xVelocity.y };
	playerDirection.Normalize();

	float wallAngle = atan2(wallNormal.x, wallNormal.z) * Rad2Deg;
	bool isWallOnLeft = playerDirection.Cross(wallNormal).y > 0.f;

	float limitAwayFromWall = wallAngle + 20.f * -((int)isWallOnLeft * 2 - 1);
	float limitTowardsWall = wallAngle + 100.f * -((int)isWallOnLeft * 2 - 1);

	FriendMath::ClampAngle(limitTowardsWall);
	FriendMath::ClampAngle(limitAwayFromWall);

	float& camera_yaw = p.cameraRotation.y;

	// Camera Lock
	{
		if (aSign < 0)
		{
			std::swap(limitTowardsWall, limitAwayFromWall);
		}

		if (!IsKnifeInPieSlice(camera_yaw, limitTowardsWall, limitAwayFromWall))
		{
			float diffTowards = abs(abs(limitTowardsWall) - abs(camera_yaw));
			float diffAway = abs(abs(limitAwayFromWall) - abs(camera_yaw));

			PrintI("Limit Camera!");
			PrintI("WallNormal: " + std::to_string(wallNormal.x) + " | " + std::to_string(wallNormal.y) + " | " + std::to_string(wallNormal.z));

			if (diffTowards > diffAway)
				camera_yaw = limitAwayFromWall;
			else
				camera_yaw = limitTowardsWall;
		}
	}

	float& camera_pitch = p.cameraRotation.x;
	if (camera_pitch > 90)
		camera_pitch = 90;
	else if (camera_pitch < -90)
		camera_pitch = -90;

	t.transform.SetEulerAngles({ camera_pitch, camera_yaw, 0 });
}