#include "pch.h"
#include "EnemySystem.h"
#include <ecs/World.h>
#include "../component/TransformComponent.h"
#include "../component/ProjectileComponent.h"
#include "../component/PlayerComponent.h"
#include "../component/AnimationDataComponent.h"
#include "../component/MeshComponent.h"
#include "../factory/ProjectileFactory.h"
#include "utility\GameHelperFunctions.h"

#include <assets\AssetDatabase.h>
#include <engine\utility\InputManager.h>
#include <engine\graphics\Animation\AnimationController.h>
#include <engine\graphics\Animation\AnimationPlayer.h>
#include <engine\graphics\Animation\Animation.h>
#include "scene\SceneCommon.h"

#include <random>

EnemySystem::EnemySystem(World* aWorld, PhysXSceneManager* aPhysXSceneManager) : System(aWorld), myPhysXSceneManager(aPhysXSceneManager)
{
	ComponentSignature enemySystemSignature;
	enemySystemSignature.set(myWorld->GetComponentSignatureID<TransformComponent>());
	enemySystemSignature.set(myWorld->GetComponentSignatureID<EnemyComponent>());
	aWorld->SetSystemSignature<EnemySystem>(enemySystemSignature);

#ifndef _RELEASE
	myRandomPoint.SetColor({ 0,0,0,1 });
	myCircleRadius.SetColor({ 1,0,0,1 });
	myPlayerPosLine.SetColor({ 0,1,1,1 });
	myRayCastLine.SetColor({ 1,0,0,1 });
#endif
}

EnemySystem::~EnemySystem()
{

}

void EnemySystem::Update(const SceneUpdateContext& aContext)
{
	
	for (Entity entity : myEntities)
	{
		EnemyComponent& enemy = myWorld->GetComponent<EnemyComponent>(entity);

		AnimationDataParameter parameter;
		AnimationPlayer maPlayerBaby;
		parameter = static_cast<int>(enemy.enemyState);

		parameter = -1;
		auto im = InputManager::GetInstance();



		if (im->IsKeyPressed('1'))
		{
			parameter = 0;
		}
		if (im->IsKeyPressed('2'))
		{
			parameter = 1;
		}
		if (im->IsKeyPressed('3'))
		{
			parameter = 2;
		}

		if (im->IsKeyPressed('4'))
		{
			parameter = 3;
		}
		if (im->IsKeyPressed('5'))
		{
			parameter = 4;
		}
		if (im->IsKeyPressed('6'))
		{
			parameter = 5;
		}


		auto& anim = myWorld->GetComponent<AnimationDataComponent>(entity);
		auto controller = AssetDatabase::GetAnimationController(myWorld->GetComponent<MeshComponent>(entity).id);

		controller->SetParameter("state", parameter, anim, anim.parameters);


		//player
		Entity playerID = myWorld->GetPlayerEntityID();

		if (playerID != INVALID_ENTITY)
		{
			TransformComponent& transformComponent = myWorld->GetComponent<TransformComponent>(entity);
			TransformComponent& playerTransform = myWorld->GetComponent<TransformComponent>(playerID);
			PlayerComponent& playerComp = myWorld->GetComponent<PlayerComponent>(playerID);
			myPlayerPos = playerTransform.transform.GetPosition();

			if (!enemy.IsTargetingPlayer)
			{
				if (isPlayerOverlaping(enemy))
				{
					enemy.IsTargetingPlayer = true;
					//	Print("[ENEMY TARGET PLAYER]");
#pragma region Overlap stuff
		//if (!enemy.IsTargetingPlayer)
		//{
		//	if (isPlayerOverlaping(enemy))
		//	{
		//		enemy.IsTargetingPlayer = true;
		//	//	Print("[ENEMY TARGET PLAYER]");

		//	}
		//}
		//else
		//{
		//	myTimer += aDT;
		//	if (myTimer >= myCheckIntervall)
		//	{
		//		myTimer = 0.0f;
		//	

		//		if (!isPlayerOverlaping(enemy))
		//		{
		//			enemy.IsTargetingPlayer = false;
		//		//	Print("[ENEMY TARGET NONE]");

		//		}
		//	}
		//}

		//if (enemy.IsTargetingPlayer)
		//{
		//	AimAtPlayer(transformComponent, aDT, enemy);

		//	//Shoot
		//	enemy.attackSpeedTimer -= aDT;
		//	//Uncomment this line if u want to see the circle clearly. remember to CTRL + K + C  func in if statement
		//	//Vector3f finalDir = CreateCircleOffsetDir(playerComp.finalVelocity.Length());
		//	if (enemy.attackSpeedTimer <= 0)
		//	{
		//		Vector3f finalDir = CreateCircleOffsetDir(playerComp.finalVelocity.Length(), enemy);
		//		enemy.attackSpeedTimer = enemy.attackSpeed;
		//		Vector3f pos = enemy.myPos;
		//		pos.y += 170.0f;
		//		ProjectileFactory::GetInstance().ShootArrow(pos, finalDir);
		//	}
		//}

#pragma endregion 


					if (Vector3f::Distance(transformComponent.transform.GetPosition(), playerTransform.transform.GetPosition()) < enemy.range)
					{
						Vector3f playerOffset = { myPlayerPos.x,myPlayerPos.y + 40.f,myPlayerPos.z };
						Vector3f preDir = playerOffset - enemy.myPos;
						PxVec3 origin = { enemy.myPos.x,enemy.myPos.y + 80,enemy.myPos.z };
						PxVec3 dir = { preDir.x,preDir.y,preDir.z };
						preDir.Normalize();
						dir.normalize();

#ifndef _RELEASE
						myRayCastLine.SetStartPosition({ enemy.myPos.x,enemy.myPos.y + 80,enemy.myPos.z });
#endif

						if (IsPlayerHit(myPhysXSceneManager->GetScene(), origin, dir, enemy.range))
						{
							enemy.enemyState = eEnemyState::TargetingPlayer;
						}
						else
						{
							enemy.enemyState = eEnemyState::Idle;
						}
					}
					switch (enemy.enemyState)
					{
					case eEnemyState::TargetingPlayer:

						AimAtPlayer(transformComponent, aContext.dt, enemy);
						//Shoot
						enemy.attackSpeedTimer -= aContext.dt;
						//Uncomment this line if u want to see the circle clearly. remember to CTRL + K + C  func in if statement
						//Vector3f finalDir = CreateCircleOffsetDir(playerComp.finalVelocity.Length());
						if (enemy.attackSpeedTimer <= 0)
						{
							maPlayerBaby.Play(anim);

							Vector3f finalDir = CreateCircleOffsetDir(playerComp.finalVelocity.Length(), enemy);
							enemy.attackSpeedTimer = enemy.attackSpeed;
							Vector3f pos = enemy.myPos;
							pos.y += 170.0f;
							ProjectileFactory::GetInstance().ShootArrow(pos, finalDir);
						}




						break;
					case eEnemyState::Idle:

						maPlayerBaby.Stop(anim);



						break;
					default:
						break;
					}
				}
			}
		}

	}
}
		void EnemySystem::AimAtPlayer(TransformComponent & aTransform, const float& aDT, EnemyComponent & aEnemy)
		{
			myDirection = (myPlayerPos - aEnemy.myPos);
			float targetRotation = atan2f(myDirection.x, myDirection.z) * 180.f / M_FRIEND_PI;

			Vector3f currentRotation = aTransform.transform.GetEulerRotation();

			float deltaAngle = targetRotation - currentRotation.y;

			if (deltaAngle > 180.f)
			{
				deltaAngle -= 360.f;
			}
			else if (deltaAngle < -180.f)
			{
				deltaAngle += 360.f;
			}

			float lerpAngle = currentRotation.y + deltaAngle;

			currentRotation = currentRotation.Lerp({ 0,lerpAngle,0 }, 5.0f * aDT);

			aTransform.transform.SetEulerAngles({ 0,currentRotation.y,0 });

		}

		Vector3f EnemySystem::CreateCircleOffsetDir(const float& aLength, EnemyComponent & aEnemy)
		{
#ifndef _RELEASE
			myPlayerPosLine.SetStartPosition(myPlayerPos);
			myPlayerPosLine.SetEndPosition({ myPlayerPos.x,myPlayerPos.y + 100 ,myPlayerPos.z });
#endif

			//TODo: maybe tweak the offset?
			myAimOffsetCircle.radius = 500.f * (-aLength / 12.f);
			myAimOffsetCircle.center = Vector3f{ myPlayerPos.x,myPlayerPos.y - 100.f,myPlayerPos.z };

			Vector3f dir = aEnemy.myPos - myAimOffsetCircle.center;
			dir.Normalize();

			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<float> dis(0, 2 * M_FRIEND_PI);

			float theta = dis(gen);

			float randomX = myAimOffsetCircle.radius * std::cos(theta);
			float randomY = myAimOffsetCircle.radius * std::sin(theta);

			Vector3f random_point(randomX, randomY, 0.0f);

			Vector3f targetDirection = aEnemy.myPos - myAimOffsetCircle.center;
			targetDirection.Normalize();

			// Rotate 
			float cosTheta = targetDirection.Dot(Vector3f(1, 0, 0));
			float sinTheta = targetDirection.Dot(Vector3f(0, 1, 0));
			float cosPhi = targetDirection.Dot(Vector3f(0, 0, 1));
			float sinPhi = std::sqrt(1 - cosPhi * cosPhi);

			Vector3f rotatedRandomPoint = {
					cosTheta * cosPhi * random_point.x - sinTheta * random_point.y + cosTheta * sinPhi * random_point.z,
					sinTheta * cosPhi * random_point.x + cosTheta * random_point.y + sinTheta * sinPhi * random_point.z,
					-sinPhi * random_point.x + cosPhi * random_point.z
			};

			// Translate the rotated point to the circle center
			Vector3f translatedPoint = myAimOffsetCircle.center + rotatedRandomPoint;
			Vector3f finalDir = translatedPoint - aEnemy.myPos;
			finalDir.Normalize();

#ifndef _RELEASE
			myRandomPoint.SetStartPosition(translatedPoint);
			myRandomPoint.SetEndPosition({ translatedPoint.x,translatedPoint.y + 80 ,translatedPoint.z });
			myCircleRadius.SetStartPosition(myAimOffsetCircle.center);
			myCircleRadius.SetEndPosition(translatedPoint);
#endif

			return finalDir;
		}

		bool EnemySystem::isPlayerOverlaping(EnemyComponent & aEnemy)
		{

			aEnemy;

			//physx::PxOverlapBuffer hit = {};
			//physx::PxVec3 halfExtents(aEnemy.sizeCollider.x, aEnemy.sizeCollider.y, aEnemy.sizeCollider.z); // Half extents of the box in each dimension
			//physx::PxBoxGeometry shapeBox(halfExtents);

			//physx::PxTransform triggerTransform(physx::PxVec3((float)aEnemy.overlapShapePos.x, (float)aEnemy.overlapShapePos.y, (float)aEnemy.overlapShapePos.z));
			// myPhysXSceneManager->GetScene()->overlap(shapeBox, triggerTransform, hit, physx::PxQueryFilterData(PxQueryFlag::eANY_HIT | physx::PxQueryFlag::eDYNAMIC));

			//	if (hit.block.actor != nullptr)
			//	{
			//		Entity playerID = myWorld->GetPlayerEntityID();
			//		PlayerComponent& playerComp = myWorld->GetComponent<PlayerComponent>(playerID);
			//	
			//		if (hit.block.actor == playerComp.controller->getActor())
			//		{
			//			return true;
			//		}
			//	}

			return false;
		}

		bool EnemySystem::IsPlayerHit(PxScene * aScene, PxVec3 aRayOrigin, PxVec3 rayDirection, float rayLength)
		{
			PxRaycastBuffer hit;
			aScene->raycast(aRayOrigin, rayDirection, rayLength, hit);

#ifndef _RELEASE
			Vector3f lineEnd = { hit.block.position.x,hit.block.position.y,hit.block.position.z };
			myRayCastLine.SetEndPosition(lineEnd);
#endif

			if (hit.block.actor != nullptr)
			{
				Entity playerID = myWorld->GetPlayerEntityID();
				auto& physXplayer = myWorld->GetComponent<PlayerComponent>(playerID);

				if (hit.block.actor == physXplayer.controller->getActor())
				{
					return true;
				}
			}

			return false;
		}

		void EnemySystem::Render()
		{
#ifndef _RELEASE
			myRandomPoint.DrawLine();
			myPlayerPosLine.DrawLine();
			myCircleRadius.DrawLine();
			myRayCastLine.DrawLine();
#endif
		}