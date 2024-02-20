#include "pch.h"
#include "EnemySystem.h"
#include <ecs/World.h>
#include "../component/TransformComponent.h"
#include "../component/ProjectileComponent.h"
#include "../component/PlayerComponent.h"
#include "../component/AnimationDataComponent.h"
#include "../component/MeshComponent.h"
#include "../factory/ProjectileFactory.h"
#include <assets\AssetDatabase.h>

#include <engine\utility\InputManager.h>
#include <engine\graphics\Animation\AnimationController.h>

EnemySystem::EnemySystem(World* aWorld) : System(aWorld)
{
}

EnemySystem::~EnemySystem()
{

}

void EnemySystem::Update(const float& aDT)
{
	for (Entity entity : myEntities)
	{

		auto& enemy = myWorld->GetComponent<EnemyComponent>(entity);
		auto& anim = myWorld->GetComponent<AnimationDataComponent>(entity);
		auto controller = AssetDatabase::GetAnimationController(myWorld->GetComponent<MeshComponent>(entity).id);
		

		AnimationDataParameter parameter{ static_cast<float>(enemy.enemyState) };
		controller->SetParameter("state", parameter, anim, anim.parameters);

		//player
		auto playerID = myWorld->GetPlayerEntityID();

		if (playerID != INVALID_ENTITY)
		{

			auto& playerTransform = myWorld->GetComponent<TransformComponent>(playerID);
			myPlayerPos = playerTransform.transform.GetPosition();

			//Orb
			TransformComponent& transformComponent = myWorld->GetComponent<TransformComponent>(entity);

			AimAtPlayer(transformComponent, aDT);

			if (InputManager::GetInstance()->IsKeyPressed('V'))
			{
				auto pos = transformComponent.transform.GetPosition();
				pos.y += 170.0f;
				ProjectileFactory::GetInstance().ShootArrow(pos, myPlayerPos - pos);
			}
		}
	}
}

void EnemySystem::AimAtPlayer(TransformComponent& aTransform, const float& aDT)
{
	myDirection = (myPlayerPos - aTransform.transform.GetPosition()).GetNormalized();
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