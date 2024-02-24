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
#include <random>

EnemySystem::EnemySystem(World* aWorld) : System(aWorld)
{
	ComponentSignature enemySystemSignature;
	enemySystemSignature.set(myWorld->GetComponentSignatureID<TransformComponent>());
	enemySystemSignature.set(myWorld->GetComponentSignatureID<EnemyComponent>());
	aWorld->SetSystemSignature<EnemySystem>(enemySystemSignature);

#ifdef _DEBUG
	myRandomPoint.SetColor({ 0,0,0,1 });
	myCircleRadius.SetColor({1,0,0,1});
	myPlayerPosLine.SetColor({ 0,1,1,1 });
#endif
}

EnemySystem::~EnemySystem()
{

}

void EnemySystem::Update(const float& aDT)
{
	for (Entity entity : myEntities)
	{
		EnemyComponent& enemy = myWorld->GetComponent<EnemyComponent>(entity);

		AnimationDataParameter parameter;
		parameter.i = static_cast<int>(enemy.enemyState);

		//auto& anim = myWorld->GetComponent<AnimationDataComponent>(entity);S
		//auto controller = AssetDatabase::GetAnimationController(myWorld->GetComponent<MeshComponent>(entity).id);
		//
		//AnimationDataParameter parameter{ static_cast<float>(enemy.enemyState) };
		//controller->SetParameter("state", parameter, anim, anim.parameters);

		//player
		Entity playerID = myWorld->GetPlayerEntityID();

		if (playerID != INVALID_ENTITY)
		{
			TransformComponent& playerTransform = myWorld->GetComponent<TransformComponent>(playerID);
			PlayerComponent& playerComp = myWorld->GetComponent<PlayerComponent>(playerID);
			TransformComponent& transformComponent = myWorld->GetComponent<TransformComponent>(entity);


			myMatrix = GetWorldTransform(myWorld, entity);
			myPos = { myMatrix.r[3].m128_f32[0],myMatrix.r[3].m128_f32[1] ,myMatrix.r[3].m128_f32[2] };
			myPlayerPos = playerTransform.transform.GetPosition();

			AimAtPlayer(transformComponent, aDT);

			//Shoot
			enemy.attackSpeedTimer -= aDT;
			//Uncomment this line if u want to see the circle clearly. remember to CTRL + K + C  func in if statement
			//Vector3f finalDir = CreateCircleOffsetDir(playerComp.finalVelocity.Length());
			if (enemy.attackSpeedTimer <= 0)
			{
				Vector3f finalDir = CreateCircleOffsetDir(playerComp.finalVelocity.Length());
				enemy.attackSpeedTimer = enemy.attackSpeed;
				Vector3f pos = myPos;
				pos.y += 170.0f;
				ProjectileFactory::GetInstance().ShootArrow(pos, finalDir);
			}
		}
	}
}

void EnemySystem::AimAtPlayer(TransformComponent& aTransform, const float& aDT)
{
	myDirection = (myPlayerPos - myPos);
	float targetRotation = atan2f(myDirection.x, myDirection.z) * 180.f / M_FRIEND_PI;

	Vector3f currentRotation = aTransform.transform.GetEulerRotation();
	float deltaAngle = targetRotation - currentRotation.z;
	if (deltaAngle > 180.f)
	{
		deltaAngle -= 360.f;
	}
	else if (deltaAngle < -180.f)
	{
		deltaAngle += 360.f;
	}
	float lerpAngle = currentRotation.z + deltaAngle;
	Vector3f newRotation(0, lerpAngle, 0);
	currentRotation = currentRotation.Lerp({ 0,0,newRotation.y }, 10.0f * aDT);
	aTransform.transform.SetEulerAngles({ 0,currentRotation.z,0 });
}

Vector3f EnemySystem::CreateCircleOffsetDir(const float& aLength)
{
#ifdef _DEBUG
	myPlayerPosLine.SetStartPosition(myPlayerPos);
	myPlayerPosLine.SetEndPosition({ myPlayerPos.x,myPlayerPos.y + 100 ,myPlayerPos.z });
#endif

	//TODo: maybe tweak the offset?
	myAimOffsetCircle.radius = 500.f * ( -aLength / 12.f);
	myAimOffsetCircle.center = Vector3f{ myPlayerPos.x,myPlayerPos.y - 100.f,myPlayerPos.z };

	Vector3f dir = myPos - myAimOffsetCircle.center;
	dir.Normalize();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0, 2 * M_FRIEND_PI);

	float theta = dis(gen);

	float randomX = myAimOffsetCircle.radius * std::cos(theta);
	float randomY = myAimOffsetCircle.radius * std::sin(theta);

	Vector3f random_point(randomX, randomY, 0.0f);

	Vector3f targetDirection = myPos - myAimOffsetCircle.center;
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
	Vector3f finalDir = translatedPoint - myPos;
	finalDir.Normalize();

#ifdef _DEBUG
	myRandomPoint.SetStartPosition(translatedPoint);
	myRandomPoint.SetEndPosition({ translatedPoint.x,translatedPoint.y + 80 ,translatedPoint.z });
	myCircleRadius.SetStartPosition(myAimOffsetCircle.center);
	myCircleRadius.SetEndPosition(translatedPoint);
#endif

	return finalDir;
}

//bool EnemySystem::IsPlayerHit(PxController* aController, PxScene* aScene, PxVec3 aRayOrigin, PxVec3 rayDirection, float rayLength)
//{
//
//
//	bool hitSomething = aScene->raycast(aRayOrigin, rayDirection,rayLength, );
//
//
//	return false;
//}

void EnemySystem::Render()
{
#ifdef _DEBUG
	myRandomPoint.DrawLine();
	myPlayerPosLine.DrawLine();
	myCircleRadius.DrawLine();
#endif
}