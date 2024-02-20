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

// temp?
#include <assets/AssetDatabase.h>
#include "..\component\AnimationDataComponent.h"
#include "..\component\MeshComponent.h"
#include <engine/graphics\animation\AnimationController.h>
#include <physics/PhysXSceneManager.h>



#include <engine/graphics/vfx/VFXManager.h>
#include <engine/graphics/vfx/VFXDatabase.h>
#include <engine/graphics/vfx/VisualEffect.h>


PlayerSystem::PlayerSystem(World* aWorld, PhysXSceneManager* aPhysXSceneManager) : System(aWorld), manager(*VFXDatabase::GetInstance())
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
	VisualEffect testEffect{};
	float tempDuration = 0.65f;
	{
		VisualEffectCell cell;
		cell.meshId = AssetDatabase::GetMeshIdFromName("sm_aoe1");
		cell.startTime = 0;
		cell.duration = tempDuration;
		cell.transform = DirectX::XMMatrixIdentity();
		cell.transform.SetPosition({ 100, 0, 0 });
		cell.type = PsType::TestVFX;
		testEffect.AddCell(cell);
	}
	{
		VisualEffectCell cell;
		cell.meshId = AssetDatabase::GetMeshIdFromName("sm_aoe1");
		cell.startTime = 0.2f;
		cell.duration = tempDuration;
		cell.transform = DirectX::XMMatrixIdentity();
		cell.transform.SetPosition({ 0, 0, 100 });
		cell.type = PsType::TestVFX;
		testEffect.AddCell(cell);
	}
	{
		VisualEffectCell cell;
		cell.meshId = AssetDatabase::GetMeshIdFromName("sm_aoe1");
		cell.startTime = 0.4f;
		cell.duration = tempDuration;
		cell.transform = DirectX::XMMatrixIdentity();
		cell.transform.SetPosition({ 0, 0, 0 });
		cell.type = PsType::TestVFX;
		testEffect.AddCell(cell);
	}
	{
		VisualEffectCell cell;
		cell.meshId = AssetDatabase::GetMeshIdFromName("sm_aoe1");
		cell.startTime = 0.6f;
		cell.duration = tempDuration;
		cell.transform = DirectX::XMMatrixIdentity();
		cell.transform.SetPosition({ 0, 0, -100 });
		cell.type = PsType::TestVFX;
		testEffect.AddCell(cell);
	}

	{
		testEffectId = VFXDatabase::GetInstance()->CreateEffect(testEffect);
		manager.InsertEffect(testEffectId);
	}

	for (auto entity : myEntities)
	{
		PlayerComponent& playerComponent = myWorld->GetComponent<PlayerComponent>(entity);
		//PhysXComponent& physXComponent = myWorld->GetComponent<PhysXComponent>(entity);

		playerComponent.callbackWrapper.OnShapeHit = [&](const PxControllerShapeHit& hit) {
			playerComponent.capsuleCollisionPosition = { (float)hit.worldPos.x, (float)hit.worldPos.y, (float)hit.worldPos.z };

			//playerComponent.isGrounded = worldPos.y - playerComponent.controller->getFootPosition().y < 1.0f && hit.worldNormal.y > 0.5f;

			playerComponent.capsuleCollisionNormal = { hit.worldNormal.x, hit.worldNormal.y, hit.worldNormal.z };
			};

		playerComponent.callbackWrapper.OnControllerHit = [&](const PxControllersHit&) {
			};

		playerComponent.callbackWrapper.OnObstacleHit = [&](const PxControllerObstacleHit&) {
			};
	}
}

#include <engine/math/helper.h>
float prevY = 0;
void PlayerSystem::Update(const float& dt)
{
	//MATI! Om myEntities ?r tom kommer denna kod inte att k?ras, det ?r b?ttre att ha allt inom denna for-loop!
	for (auto entity : myEntities)
	{
		PlayerComponent& p = myWorld->GetComponent<PlayerComponent>(entity);
		TransformComponent& t = myWorld->GetComponent<TransformComponent>(entity);

		Input(entity, dt);
		Collision(entity, dt);
		VaultCollision(entity, dt);
		CameraMove(entity, dt);

		Slide(entity, dt);
		Move(entity, dt);

		TestVfxBlob(dt);

		{
			PxControllerFilters filter = PxControllerFilters();
			filter.mCCTFilterCallback = nullptr;
			filter.mFilterCallback = nullptr;
			filter.mFilterFlags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC;

			p.groundCollisionNormal = { 0, 0, 0 };
			p.capsuleCollisionPosition = { 0, 0, 0 };
			p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };
			p.finalVelocity *= 60.0f;
			p.finalVelocity *= dt;
			p.isGrounded = false;

			p.controller->move(PxVec3(p.finalVelocity.x, p.finalVelocity.y, p.finalVelocity.z), 1.0f, dt, filter);
		}

		auto footPos = p.controller->getFootPosition();
		t.transform.SetPosition({ (float)footPos.x, (float)footPos.y, (float)footPos.z });
	}
}

void PlayerSystem::Render()
{
	manager.Render();
}

void PlayerSystem::Input(Entity entity, float)
{
	PlayerComponent& p = myWorld->GetComponent<PlayerComponent>(entity);
	InputManager* im = InputManager::GetInstance();

	p.inputDirection = { 0, 0 };
	Matrix4x4f rot = Matrix4x4f::CreateRollPitchYawMatrix(0, p.cameraRotation.y, 0);
	Vector3f right = { rot(1, 1), rot(1, 2), rot(1, 3) };
	Vector3f forward = { rot(3, 1), rot(3, 2), rot(3, 3) };
	if (im->IsKeyHeld('W'))
	{
		p.inputDirection += { forward.x, forward.z };
	}
	if (im->IsKeyHeld('S'))
	{
		p.inputDirection += { -forward.x, -forward.z };
	}
	if (im->IsKeyHeld('A'))
	{
		p.inputDirection += { right.x, right.z };
	}
	if (im->IsKeyHeld('D'))
	{
		p.inputDirection += { -right.x, -right.z };
	}

	p.hasPressedJump = im->IsKeyPressed(VK_SPACE);

	p.inputDirection.Normalize();

	p.isHoldingCrouch = im->IsKeyHeld(VK_SHIFT);
}

void PlayerSystem::Move(Entity entity, float dt)
{
	PlayerComponent& p = myWorld->GetComponent<PlayerComponent>(entity);

	if (p.isSliding) { return; }

	//InputManager* im = InputManager::GetInstance();


	//p.controller->resize


	{
		// Y
		{
			p.isWallRunning = false;
			float gravity = p.gravity * dt;
			if (p.isGrounded)
			{
				p.yVelocity = 0;
			}
			else if (p.wallNormal.LengthSqr() > 0 && p.yVelocity + gravity <= 0)
			{
				gravity *= p.wallRunGravityMultiplier;
				p.isWallRunning = true;
			}
			else if (p.groundCollisionNormal.y < -0.5f && p.yVelocity > 0)
			{
				p.yVelocity = 0;
			}

			p.yVelocity += gravity;


			if (p.hasPressedJump)
			{
				if (p.isVaultable)
				{
					p.controller->setFootPosition({ p.vaultLocation.x, p.vaultLocation.y, p.vaultLocation.z });
					p.yVelocity = 0;
				}
				else if (p.isGrounded || p.isWallRunning)
				{
					p.yVelocity = p.jumpSpeed;
					if (p.isWallRunning)
					{
						//float length = p.xVelocity.Length();
						p.xVelocity += Vector2f(p.wallNormal.x, p.wallNormal.z) * p.wallJumpSpeed;
						p.isWallRunning = false;
					}
				}
			}
		}

		bool shouldUseGroundFriction = p.isGrounded || p.isWallRunning;
		float airMultiplier = (shouldUseGroundFriction ? 1.0f : p.airMobilityMultiplier);
		// X/Z
		{
			float strafeMultiplier = 1.0f;
			if (!shouldUseGroundFriction && p.xVelocity.LengthSqr() > 0)
			{
				strafeMultiplier = 1.0f - p.inputDirection.Dot(p.xVelocity.GetNormalized());
			}
			if (!p.isWallRunning)
			{
				float crouchingMultiplier = p.isCrouching ? p.crouchSpeedMultiplier : 1.0f;
				p.xVelocity += p.inputDirection * (p.speed) * strafeMultiplier * airMultiplier * crouchingMultiplier;
			}

			float friction = shouldUseGroundFriction ? p.friction : p.airFriction;
			if (p.xVelocity.Length() - friction < 0.01f)
			{
				p.xVelocity = { 0, 0 };
			}
			else if (!p.isWallRunning)
			{
				Vector2f r = p.xVelocity.GetNormalized() * friction;
				p.xVelocity -= r;
			}
			//else
			//{
			//	Vector2f r = p.xVelocity.GetNormalized() * friction;
			//	p.xVelocity -= r;
			//}


			{
				float length = p.xVelocity.Length();
				if (p.isGrounded && length > p.maxSpeed)
				{
					float delta = length - p.maxSpeed;
					p.xVelocity -= p.xVelocity.GetNormalized() * delta * 0.3f;
					//p.xVelocity.Normalize();
					//p.xVelocity *= p.maxSpeed;
				}
			}

			if (p.isWallRunning)
			{
				float length = p.xVelocity.Length();
				Vector3f wallDirection = p.wallNormal.Cross(Vector3f::Up()).GetNormalized();
				Vector3f projectedVector = Vector3f::Project(Vector3f(p.xVelocity.x, 0, p.xVelocity.y), wallDirection);
				projectedVector.Normalize();

				int sign = FriendMath::Sign(projectedVector.Dot(wallDirection));
				p.xVelocity = Vector2f(wallDirection.x, wallDirection.z) * length * (float)sign;
				wallDirection;
				length;
			}
		}

		//physx::PxControllerState state;
		//p.controller->getState(state);
		//state.collisionFlags;

		//prevY = (float)p.controller->getPosition().y;
	}
}

void PlayerSystem::CameraMove(Entity entity, float dt)
{
	PlayerComponent& p = myWorld->GetComponent<PlayerComponent>(entity);
	TransformComponent& t = myWorld->GetComponent<TransformComponent>(entity);

	float& camera_pitch = p.cameraRotation.x;
	float& camera_yaw = p.cameraRotation.y;

	InputManager* im = InputManager::GetInstance();
	Vector2<float> mouseMovement = im->GetCurrentMousePositionVector2f();

	float rotation_rate_x = p.cameraSensitivity.x * dt;
	float rotation_rate_y = p.cameraSensitivity.y * dt;

	camera_yaw += mouseMovement.x * rotation_rate_x;
	camera_pitch += mouseMovement.y * rotation_rate_y;

	camera_yaw = std::fmodf(camera_yaw, 360);
	if (camera_pitch > 90)
		camera_pitch = 90;
	else if (camera_pitch < -90)
		camera_pitch = -90;

	t.transform.SetEulerAngles({ camera_pitch, camera_yaw, 0 });
}

void PlayerSystem::Collision(Entity entity, float)
{
	PlayerComponent& p = myWorld->GetComponent<PlayerComponent>(entity);

	// Ground Collision
	if (p.yVelocity <= 0)
	{
		physx::PxOverlapBuffer hit = {};
		auto footPos = p.controller->getFootPosition();
		physx::PxTransform shapePose(physx::PxVec3((float)footPos.x, (float)footPos.y + 25.0f, (float)footPos.z));

		PxSweepBuffer hitBuffer;
		if (myPhysXSceneManager->GetScene()->sweep(
			physx::PxSphereGeometry(40.0f),
			shapePose,
			physx::PxVec3(0, -1, 0),
			100.0f,
			hitBuffer,
			physx::PxHitFlag::eDEFAULT,
			physx::PxQueryFilterData(PxQueryFlag::eANY_HIT | physx::PxQueryFlag::eSTATIC)
		))
		{
			p.groundCollisionNormal = { hitBuffer.block.normal.x, hitBuffer.block.normal.y, hitBuffer.block.normal.z };
			p.isGrounded = p.groundCollisionNormal.y > 0.5f && (p.controller->getFootPosition().y - p.capsuleCollisionPosition.y < 15.0f);
		}
	}


	// Wall collision
	{
		physx::PxRaycastBuffer hit = {};

		auto footPos = p.controller->getFootPosition();
		physx::PxTransform shapePose(physx::PxVec3((float)footPos.x, (float)footPos.y + 25.0f, (float)footPos.z));    // [in] initial shape pose (at distance=0)

		Vector3f normal = { 0, 0, 0 };

		if (p.capsuleCollisionNormal.LengthSqr() > 0 && abs(p.capsuleCollisionNormal.Dot(Vector3f::Up())) < 0.1f)
		{
			normal = p.capsuleCollisionNormal;
		}
		else if (p.wallNormal.LengthSqr() > 0)
		{
			normal = p.wallNormal;
		}

		p.wallNormal = { 0, 0, 0 };

		if (normal.LengthSqr())
		{
			auto pos = p.controller->getPosition();
			if (myPhysXSceneManager->GetScene()->raycast(
				physx::PxVec3((float)pos.x, (float)pos.y, (float)pos.z),
				physx::PxVec3(-normal.x, -normal.y, -normal.z),
				p.wallRunRaycastLength,
				hit,
				physx::PxHitFlag::eDEFAULT,
				physx::PxQueryFilterData(PxQueryFlag::eANY_HIT | physx::PxQueryFlag::eSTATIC)
			)
				)
			{
				p.wallNormal = { hit.block.normal.x,  hit.block.normal.y,  hit.block.normal.z };
			}
		}
	}

}

void PlayerSystem::VaultCollision(Entity entity, float)
{
	PlayerComponent& p = myWorld->GetComponent<PlayerComponent>(entity);

	// Vault collision
	p.isVaultable = false;
	{
		physx::PxRaycastBuffer hit = {};

		auto footPos = p.controller->getFootPosition();
		physx::PxTransform shapePose(physx::PxVec3((float)footPos.x, (float)footPos.y + p.cameraHeight, (float)footPos.z));    // [in] initial shape pose (at distance=0)

		Matrix3x3f rotationMatrix = Matrix3x3f::CreateRotationAroundY(p.cameraRotation.y * Deg2Rad);
		Vector3f rot = Vector3f::Forward() * rotationMatrix;
		rot.Normalize();
		physx::PxVec3 dir = { rot.x, rot.y, rot.z };
		auto wallDetectionOrigin = shapePose.p + dir * CHARACTER_RADIUS;
		auto vaultCastOrigin = wallDetectionOrigin + dir * p.vaultRange - dir;


		if (myPhysXSceneManager->GetScene()->raycast(
			wallDetectionOrigin,
			dir,
			p.vaultRange,
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
			p.cameraHeight - STEP_OFFSET - 1.0f,
			vaultHit,
			physx::PxHitFlag::eDEFAULT,
			physx::PxQueryFilterData(PxQueryFlag::eANY_HIT | physx::PxQueryFlag::eSTATIC)
		)
			)
		{
			p.isVaultable = vaultHit.block.normal.y > 0.75f;
			p.vaultLocation = { (float)vaultHit.block.position.x, (float)vaultHit.block.position.y, (float)vaultHit.block.position.z };
		}
	}
}

void PlayerSystem::Slide(Entity entity, float dt)
{
	PlayerComponent& p = myWorld->GetComponent<PlayerComponent>(entity);

	bool canUncrouch = true;
	if (p.isCrouching || p.isSliding)
	{
		physx::PxOverlapBuffer hit = {};
		auto footPos = p.controller->getFootPosition();
		physx::PxTransform shapePose(physx::PxVec3((float)footPos.x, (float)footPos.y + CHARACTER_HEIGHT_PRE_ADJUSTMENT, (float)footPos.z));

		if (p.groundCollisionNormal.LengthSqr() > 0)
		{
			if (myPhysXSceneManager->GetScene()->overlap(
				physx::PxSphereGeometry(CHARACTER_RADIUS - 1 + HEAD_BONK_OFFSET),
				shapePose,
				hit,
				physx::PxQueryFilterData(PxQueryFlag::eANY_HIT | physx::PxQueryFlag::eSTATIC)
			))
			{
				canUncrouch = false;
			}
		}
	}

	if (p.hasPressedJump && canUncrouch)
	{
		p.isSliding = false;
		p.isCrouching = false;
		p.slideTimer = 0;
		p.controller->resize(CHARACTER_HEIGHT);
		return;
	}

	if (p.isSliding || p.isCrouching)
	{
		p.controller->resize(0.0f);
	}
	else if (canUncrouch)
	{
		p.controller->resize(CHARACTER_HEIGHT);
	}

	if ((!p.isHoldingCrouch || !p.isGrounded || p.isWallRunning) && canUncrouch)
	{
		p.slideTimer = 0;
		p.isCrouching = false;
		p.isSliding = false;

		if (false) // TODO: Check if there is a wall above of the player
		{
			p.isCrouching = false;
			return;
		}

		return;
	}

	if (p.isCrouching)
	{
		p.slideTimer = 0;
		p.isSliding = false;
		return;
	}

	p.isSliding = p.xVelocity.Length() >= p.slideSpeedThreshold;
	if (!p.isSliding)
	{
		p.isCrouching = true;
		p.slideTimer = 0;
		return;
	}

	if (p.slideTimer <= 0)
	{
		p.xVelocity *= p.slideSpeed;
	}

	p.slideTimer += dt;

	if (p.slideTimer >= p.slideTimeUnitlFriction)
	{
		p.xVelocity *= p.slideFriction;
	}
}


void PlayerSystem::TestVfxBlob(const float& dt)
{
	InputManager* im = InputManager::GetInstance();
	if (im->IsKeyPressed('P'))
	{
		manager.Play(0);
	}
	else if (im->IsKeyPressed('O'))
	{
		manager.Stop(0);
	}
	manager.Update(dt);
}
