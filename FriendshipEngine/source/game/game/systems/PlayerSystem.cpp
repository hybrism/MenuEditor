#include "pch.h"
#include "PlayerSystem.h"
#include <engine/utility/Error.h>
#include <ecs/World.h>
#include <iostream>
#include <algorithm>
#include "../component/PlayerComponent.h"
#include "../component/TransformComponent.h"
#include "../component/ColliderComponent.h"
#include "../component/CollisionDataComponent.h"

#include <engine/utility/InputManager.h>

#include <engine/debug/DebugLine.h>
#include <engine/math/Intersection.h>

#include <imgui/imgui.h>
#include <random>

// temp?
#include <assets/AssetDatabase.h>
#include "..\component\AnimationDataComponent.h"
#include "..\component\MeshComponent.h"
#include <engine/graphics\animation\AnimationController.h>
#include <physics/PhysXSceneManager.h>
#include "audio/NewAudioManager.h"
#include <engine/graphics/Camera.h>


PlayerSystem::PlayerSystem(World* aWorld, PhysXSceneManager* aPhysXSceneManager) : System(aWorld)
{
	myPhysXSceneManager = aPhysXSceneManager;

	ComponentSignature signature;
	signature.set(myWorld->GetComponentSignatureID<TransformComponent>());
	signature.set(myWorld->GetComponentSignatureID<PlayerComponent>());

	aWorld->SetSystemSignature<PlayerSystem>(signature);
}

PlayerSystem::~PlayerSystem()
{
}

#include <iostream>
#include <assets/AssetDatabase.h>
void PlayerSystem::Init()
{
	myMouseSensitivyMultiplier = GraphicsEngine::GetInstance()->DX().GetViewportDimensions().x / 1920.0f;



	for (auto entity : myEntities)
	{
		PlayerComponent& playerComponent = myWorld->GetComponent<PlayerComponent>(entity);
		TransformComponent& transformComponent = myWorld->GetComponent<TransformComponent>(entity);
		//PhysXComponent& physXComponent = myWorld->GetComponent<PhysXComponent>(entity);

		PxExtendedVec3 vec3 = { playerComponent.SpawnPoint.x,playerComponent.SpawnPoint.y,playerComponent.SpawnPoint.z };
		playerComponent.controller->setFootPosition(vec3);
		playerComponent.cameraRotation.y = playerComponent.startGameRot.y;
		transformComponent.transform.SetEulerAngles({ 0.f,playerComponent.startGameRot.y,0.f });

		{
			myStateMachine.Init();
			auto& animData = myWorld->GetComponent<AnimationDataComponent>(entity);
			PlayerStateUpdateContext context{ 0, playerComponent, transformComponent, animData, myPhysXSceneManager, AssetDatabase::GetAnimationController(animData.controllerId) };
			myStateMachine.SetState(ePlayerClassStates::Airborne, context);
		}

		//playerComponent.currentPlayerState = myPlayerStateArray[static_cast<int>(ePlayerClassStates::GroundState)];
		//SetPlayerState(playerComponent, ePlayerClassStates::GroundState);
		playerComponent.callbackWrapper.OnShapeHit = [&](const PxControllerShapeHit& hit)
			{
				playerComponent.collision.capsulePosition = { (float)hit.worldPos.x, (float)hit.worldPos.y, (float)hit.worldPos.z };

				playerComponent.collision.capsuleNormal = { hit.worldNormal.x, hit.worldNormal.y, hit.worldNormal.z };

				playerComponent.isGrounded = hit.worldPos.y - playerComponent.controller->getFootPosition().y < CHARACTER_RADIUS && Vector3f::Up().Dot(playerComponent.collision.capsuleNormal) > 0.7f;

			};

		playerComponent.callbackWrapper.OnControllerHit = [&](const PxControllersHit&)
			{
			};

		playerComponent.callbackWrapper.OnObstacleHit = [&](const PxControllerObstacleHit&)
			{
			};

	}

	filter = PxControllerFilters();
}

#include <engine/math/helper.h>
float prevY = 0;
void PlayerSystem::Update(SceneUpdateContext& aContext)
{
#ifndef _RELEASE
	if (GraphicsEngine::GetInstance()->GetCamera() != GraphicsEngine::GetInstance()->GetViewCamera())
	{
		return;
	}
#endif

	//MATI! Om myEntities ?r tom kommer denna kod inte att k?ras, det ?r b?ttre att ha allt inom denna for-loop!
	for (auto entity : myEntities)
	{
		PlayerComponent& p = myWorld->GetComponent<PlayerComponent>(entity);
		TransformComponent& t = myWorld->GetComponent<TransformComponent>(entity);
		if (p.isDead)
			Respawn(entity);



#ifdef _EDITOR
		if (!myWorld->TryGetComponent<AnimationDataComponent>(entity))
		{
			PrintW("The Player MUST have a a skeletal mesh to function");
			return;
		}
#endif
		AnimationDataComponent& animationData = myWorld->GetComponent<AnimationDataComponent>(entity);
		AnimationController& controller = AssetDatabase::GetAnimationController(animationData.controllerId);
		//if (InputManager::GetInstance()->IsMouseVisible())
		//{
		//	return;
		//}
		//p.currentPlayerState->Update(dt);
		//AudioManager::GetInstance()->SetPlayerListenPosition(t.transform);


		if (p.health.playerHealth <= 0)
		{
			p.health.playerHealth = p.health.playerMaxHealth;
			Respawn(entity);
		}

		Input(entity, aContext.dt);
		Collision(entity, aContext.dt);
		VaultCollision(entity, aContext.dt);
		CameraMove(entity, aContext.dt);

		PlayerStateUpdateContext context{ aContext.dt, p, t, animationData, myPhysXSceneManager, controller };
		myStateMachine.Update(context);
		p.currentPlayerState = myStateMachine.GetCurrentState();



		if (p.xVelocity.Length() >= PlayerConstants::godSpeedThreshhold)
		{
			p.timer.godSpeedTimer += aContext.dt;
			if (p.timer.godSpeedTimer > 5.f)
			{
				p.finalVelocity += {p.xVelocity.x * 0.1f, 0, p.xVelocity.y * 0.1f };
				p.isGodSpeed = true;
			}
		}
		else
		{
			if (p.currentPlayerState != ePlayerClassStates::Vault)
			{
				p.timer.godSpeedTimer = 0;
				p.isGodSpeed = false;
			}
		}

		//if (AudioManager::GetInstance()->IsEventPlaying(FSPRO::Event::Freebird))
		//{
		//	std::string jumpFilePath = RELATIVE_AUDIO_ASSET_PATH;
		//	jumpFilePath += "Jump.wav";
		//	std::cout << "Free the birds!" << std::endl;
		//}


		//p.controller->setFootPosition({ p.vaultLocation.x, p.vaultLocation.y, p.vaultLocation.z });
		//p.yVelocity = 0;

		{
			//p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };

			PxFilterData filterData;

			filter.mCCTFilterCallback = nullptr;
			filter.mFilterCallback = nullptr;
			filter.mFilterFlags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC;
			filter.mFilterData = &filterData;

			p.isGrounded = false;
			p.collision.groundNormal = { 0, 0, 0 };
			p.collision.capsulePosition = { 0, 0, 0 };
			p.collision.previousCapsuleNormal = p.collision.capsuleNormal;
			p.collision.capsuleNormal = { 0, 0, 0 };




			// THIS IS NEEDED, SINCE ALL METRICS ARE DEVELOPED AROUND 60 FPS IT IS MULTIPLIED BY 60 FOR NOW
			// ADDITIONALLY, WE NEED TO MULTIPLY BY DT TO GET THE CORRECT VELOCITY
			p.finalVelocity *= 60.0f;
			p.finalVelocity *= aContext.dt;
			p.controller->move(PxVec3(p.finalVelocity.x, p.finalVelocity.y, p.finalVelocity.z), 0.01f, aContext.dt, filter);
		}

		MeshComponent& meshComponent = myWorld->GetComponent<MeshComponent>(entity);

		float meshHeight = p.currentCameraHeight != PlayerConstants::cameraHeight ? PlayerConstants::meshCrouchHeight : PlayerConstants::meshHeight;
		meshComponent.offset.SetPosition({ 0, meshHeight, 0 });
		controller.SetParameter(static_cast<size_t>(PlayerAnimationParameter::eXSpeed), { p.xVelocity.Length() }, animationData, animationData.parameters);
		controller.SetParameter(static_cast<size_t>(PlayerAnimationParameter::eYSpeed), { p.yVelocity }, animationData, animationData.parameters);
		//controller->SetParameter(1, { p.isSliding }, animationData, animationData.parameters);

		auto footPos = p.controller->getFootPosition();
		t.transform.SetPosition({ (float)footPos.x, (float)footPos.y, (float)footPos.z });

		static float timermax = .45f;
		static float timercounter = 0;
		static float extra = 0;

		if (p.currentPlayerState == ePlayerClassStates::Ground && p.finalVelocity.Length() > 0.4f)
		{
			timercounter += aContext.dt + (p.finalVelocity.Length() / 3000.f) + extra;
			extra += aContext.dt / 800;
			if (timercounter > timermax)
			{
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<int> dis(1, 50);

				int randomNumber = dis(gen);


				timercounter = 0;
				randomNumber;
				NewAudioManager::GetInstance()->PlaySound(eSounds::Run, 0.35f, 0.65f + (randomNumber / 100.f)); //(float)randomNumber / 100.f
				if (extra > 0.01)
				{
					extra = 0.01f;
				}
			}
		}
		else if (p.currentPlayerState == ePlayerClassStates::Ground && p.finalVelocity.Length() < 0.3f)
		{
			extra = 0;
		}
		NewAudioManager::GetInstance()->SetPlayerListenPosition(t.transform);
		//std::cout << "Player Pos :" << t.transform.GetPosition().x << "  " << t.transform.GetPosition().y << " \n";
	}
}

void PlayerSystem::Input(Entity entity, float)
{
	PlayerComponent& p = myWorld->GetComponent<PlayerComponent>(entity);
	InputManager* im = InputManager::GetInstance();

	p.input.direction = { 0, 0 };
	Matrix4x4f rot = Matrix4x4f::CreateRollPitchYawMatrix(0, p.cameraRotation.y, 0);
	Vector3f right = { rot(1, 1), rot(1, 2), rot(1, 3) };
	Vector3f forward = { rot(3, 1), rot(3, 2), rot(3, 3) };
	if (im->IsKeyHeld('W'))
	{
		p.input.direction += { forward.x, forward.z };
	}
	if (im->IsKeyHeld('S'))
	{
		p.input.direction += { -forward.x, -forward.z };
	}
	if (im->IsKeyHeld('A'))
	{
		p.input.direction += { right.x, right.z };
	}
	if (im->IsKeyHeld('D'))
	{
		p.input.direction += { -right.x, -right.z };
	}
	if (im->IsLeftMouseButtonPressed())
	{
		float pitch = 0.9f + (GraphicsEngine::GetInstance()->GetCamera()->GetTransform().GetForward().y / 5.f)  ;




		NewAudioManager::GetInstance()->PlayLoopingSound(eDedicatedChannels::Honk, eSounds::Honk, 1.f, pitch);
	}
	p.input.hasPressedJump = im->IsKeyPressed(VK_SPACE);

	if (p.input.hasPressedJump && p.currentPlayerState != ePlayerClassStates::Airborne)
	{
		NewAudioManager::GetInstance()->PlaySound(eSounds::Jump, 1.0f);
		//NewAudioManager::GetInstance()->StopMusic();
	}

	p.input.isHoldingJump = im->IsKeyHeld(VK_SPACE);

	p.input.direction.Normalize();

	p.input.isHoldingCrouch = im->IsKeyHeld(VK_SHIFT);
}

void PlayerSystem::CameraMove(Entity entity, float dt)
{
	PlayerComponent& p = myWorld->GetComponent<PlayerComponent>(entity);
	TransformComponent& t = myWorld->GetComponent<TransformComponent>(entity);

	float& camera_pitch = p.cameraRotation.x;
	float& camera_yaw = p.cameraRotation.y;

	InputManager* im = InputManager::GetInstance();
	Vector2f mouseMovement = im->GetCurrentMousePositionVector2f();

	float rotation_rate_x = p.cameraSensitivity.x * dt * myMouseSensitivyMultiplier;
	float rotation_rate_y = p.cameraSensitivity.y * dt * myMouseSensitivyMultiplier;

	camera_yaw += mouseMovement.x * rotation_rate_x;
	camera_pitch += mouseMovement.y * rotation_rate_y;

	FriendMath::ClampAngle(camera_yaw);

	//PrintI("CameraAngle: " + std::to_string(camera_yaw));

	if (camera_pitch > 89)
		camera_pitch = 89;
	else if (camera_pitch < -89)
		camera_pitch = -89;
	//{
	//	int sign = FriendMath::Sign(camera_pitch);
	//	if (camera_pitch * sign > 180)
	//	{
	//		camera_pitch -= 360 * (int)(abs(camera_pitch / 360) + 1) * sign;
	//	}
	//}

	t.transform.SetEulerAngles({ camera_pitch, camera_yaw, 0 });
}

void PlayerSystem::Collision(Entity entity, float)
{
	PlayerComponent& p = myWorld->GetComponent<PlayerComponent>(entity);

	// Ground Collision
	if (p.yVelocity <= 0)
	{
		physx::PxVec3 footPos = physx::toVec3(p.controller->getFootPosition());
		footPos = physx::PxVec3((float)footPos.x, (float)footPos.y + CHARACTER_HEIGHT / 2, (float)footPos.z);
		physx::PxRaycastBuffer hit = {};
		if (myPhysXSceneManager->GetScene()->raycast(
			footPos,
			physx::PxVec3(0, -1, 0),
			CHARACTER_HEIGHT / 2 + 10.0f,
			hit,
			physx::PxHitFlag::eDEFAULT,
			physx::PxQueryFilterData(PxQueryFlag::eANY_HIT | physx::PxQueryFlag::eSTATIC)
		)
			)
		{
			p.collision.groundNormal = { hit.block.normal.x, hit.block.normal.y, hit.block.normal.z };
			p.isGrounded = p.collision.groundNormal.y > 0.5f;
		}
	}



	// Wall collision
	{
		physx::PxRaycastBuffer hit = {};

		auto footPos = p.controller->getFootPosition();
		physx::PxTransform shapePose(physx::PxVec3((float)footPos.x, (float)footPos.y + 25.0f, (float)footPos.z));    // [in] initial shape pose (at distance=0)

		p.collision.wallNormal = { 0, 0, 0 };
		if (p.collision.capsuleNormal.LengthSqr() == 0 && p.xVelocity.Length() > PlayerConstants::minWallRunSpeed)
		{
			//Vector3f dir = p.collision.previousCapsuleNormal * -1.0f;
			Vector3f dir = p.finalVelocity;
			dir.y = 0.0f;
			dir.Normalize();
			//if (p.collision.previousCapsuleNormal.LengthSqr() == 0)
			//{
			//	dir = p.finalVelocity;
			//	dir.y = 0.0f;
			//	dir.Normalize();
			//}


			// is wallrunning = false
			auto ppp = p.controller->getPosition();
			if (myPhysXSceneManager->GetScene()->raycast(
				{ (float)ppp.x, (float)ppp.y, (float)ppp.z },
				{ dir.x, dir.y, dir.z },
				PlayerConstants::wallRunDetectionRaycastLength,
				hit,
				physx::PxHitFlag::eDEFAULT,
				physx::PxQueryFilterData(PxQueryFlag::eANY_HIT | physx::PxQueryFlag::eSTATIC)
			)
				)
			{
				p.collision.wallNormal = { hit.block.normal.x,  hit.block.normal.y,  hit.block.normal.z };
			}
		}
	}

	//{
	//	if ((p.collision.capsuleNormal.x != 0 || p.collision.capsuleNormal.z != 0) && p.collision.capsuleNormal.y < 0.1f)
	//	{
	//		PrintI("Capsule Collision" + std::to_string(p.collision.capsuleNormal.x) + " " + std::to_string(p.collision.capsuleNormal.y) + " " + std::to_string(p.collision.capsuleNormal.z));
	//		Vector2f wallSlow = { p.finalVelocity.x, p.finalVelocity.z };
	//		wallSlow /= 5.f;
	//		p.finalVelocity.x -= wallSlow.x;
	//		p.finalVelocity.z -= wallSlow.y;
	//	}

	//}

}

void PlayerSystem::VaultCollision(Entity entity, float)
{
	PlayerComponent& p = myWorld->GetComponent<PlayerComponent>(entity);

	// Vault collision
	p.isVaultable = false;
	{
		physx::PxRaycastBuffer hit = {};

		auto footPos = p.controller->getFootPosition();
		physx::PxTransform shapePose(physx::PxVec3((float)footPos.x, (float)footPos.y + PlayerConstants::cameraHeight, (float)footPos.z));    // [in] initial shape pose (at distance=0)

		Matrix3x3f rotationMatrix = Matrix3x3f::CreateRotationAroundY(p.cameraRotation.y * Deg2Rad);
		Vector3f rot = Vector3f::Forward() * rotationMatrix;
		rot.Normalize();
		physx::PxVec3 dir = { rot.x, rot.y, rot.z };
		auto wallDetectionOrigin = shapePose.p + dir * CHARACTER_RADIUS;
		auto vaultCastOrigin = wallDetectionOrigin + dir * PlayerConstants::vaultRange - dir;


		if (myPhysXSceneManager->GetScene()->raycast(
			wallDetectionOrigin,
			dir,
			PlayerConstants::vaultRange,
			hit,
			physx::PxHitFlag::eDEFAULT,
			physx::PxQueryFilterData(PxQueryFlag::eANY_HIT | physx::PxQueryFlag::eSTATIC)
		)
			)
		{
			vaultCastOrigin = hit.block.position - dir;
		}

		//hit = {};
		physx::PxRaycastBuffer vaultHit = {};
		if (myPhysXSceneManager->GetScene()->raycast(
			vaultCastOrigin,
			physx::PxVec3(0, -1, 0),
			PlayerConstants::cameraHeight - STEP_OFFSET - 1.0f,
			vaultHit,
			physx::PxHitFlag::eDEFAULT,
			physx::PxQueryFilterData(PxQueryFlag::eANY_HIT | physx::PxQueryFlag::eSTATIC)
		)
			)
		{
			p.isVaultable = vaultHit.block.normal.y > 0.75f;
			p.vaultRayCastOrigin = { (float)vaultCastOrigin.x, (float)vaultCastOrigin.y, (float)vaultCastOrigin.z };
			p.vaultLocation = { (float)vaultHit.block.position.x, (float)vaultHit.block.position.y, (float)vaultHit.block.position.z };
		}
	}
}

void PlayerSystem::Respawn(Entity entity)
{
	PlayerComponent& p = myWorld->GetComponent<PlayerComponent>(entity);
	p.cameraRotation = { p.SpawnRot.x, p.SpawnRot.y };
	p.finalVelocity = { 0, 0, 0 };
	PxExtendedVec3 vec3 = { p.SpawnPoint.x,p.SpawnPoint.y,p.SpawnPoint.z };
	p.controller->setFootPosition(vec3);
	p.isDead = false;
}

