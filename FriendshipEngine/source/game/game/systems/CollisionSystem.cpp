#include "pch.h"

#include "CollisionSystem.h"
#include <ecs/World.h>
#include <engine/utility/Error.h>
#include <engine/math/Vector3.h>

#include <fmod/fmod.h>

#include "..\component\TransformComponent.h"
#include "..\component\ColliderComponent.h"
#include "..\component\CollisionDataComponent.h"
#include "..\component\PlayerComponent.h"

#include <engine/graphics/Camera.h>
#include <engine/graphics/GraphicsEngine.h>
#include <engine/graphics/renderer/DebugRenderer.h>
#include <engine/utility/InputManager.h>
#include <engine/math/Intersection.h>

CollisionSystem::CollisionSystem(World* aWorld) : System(aWorld)
{
	ComponentSignature signature;
	signature.set(myWorld->GetComponentSignatureID<TransformComponent>());
	signature.set(myWorld->GetComponentSignatureID<ColliderComponent>());
	signature.set(myWorld->GetComponentSignatureID<CollisionDataComponent>());
	aWorld->SetSystemSignature<CollisionSystem>(signature);

	SetupCollisionLayers();
}

CollisionSystem::~CollisionSystem()
{
}

void CollisionSystem::Update(SceneUpdateContext& /*dt*/)
{
	Entity playerID = myWorld->GetPlayerEntityID();
	if (playerID == INVALID_ENTITY)
		return;

	//Reset all colliders
	for (auto& entity : myEntities)
	{
		auto& cd = myWorld->GetComponent<CollisionDataComponent>(entity);
		cd.isColliding = false;
	}

	for (auto& entity : myEntities)
	{
		//TODO: Fix layer check
		//if (LayerCheck(firstEntity, playerID) == false)
		//	continue;

		if (entity == playerID)
			continue;

		//So we dont check collision on all entities only the ones which are close enough, -> myDistanceToCheckCollision <-
		if (IsCloseEnoughToCheckCollision(entity, playerID) == false)
			continue;

		if (IsColliding(entity, playerID))
		{
			myWorld->GetComponent<CollisionDataComponent>(entity).isColliding = true;
			myWorld->GetComponent<CollisionDataComponent>(playerID).isColliding = true;
		}
	}
}

void CollisionSystem::Render()
{
#ifdef _EDITOR
	DrawAllColliders();
#endif
}

bool CollisionSystem::IsCloseEnoughToCheckCollision(Entity aFirstEntity, Entity aSecondEntity)
{
	float distanceBetweenEntities = Vector3f::Distance(myWorld->GetComponent<TransformComponent>(aFirstEntity).transform.GetPosition(), myWorld->GetComponent<TransformComponent>(aSecondEntity).transform.GetPosition());
	if (distanceBetweenEntities <= myDistanceToCheckCollision)
	{
		return true;
	}
	return false;
}

bool CollisionSystem::IsColliding(Entity aFirstEntityIndex, Entity aSecondEntityIndex)
{
	auto& firstTransform = myWorld->GetComponent<TransformComponent>(aFirstEntityIndex);
	auto& firstCollider = myWorld->GetComponent<ColliderComponent>(aFirstEntityIndex);

	auto& secondTransform = myWorld->GetComponent<TransformComponent>(aSecondEntityIndex);
	auto& secondCollider = myWorld->GetComponent<ColliderComponent>(aSecondEntityIndex);

	Vector3f firstMin = firstCollider.aabb3D.GetMin();
	Vector3f firstMax = firstCollider.aabb3D.GetMax();
	firstMin += firstTransform.transform.GetPosition();
	firstMax += firstTransform.transform.GetPosition();

	Vector3f secondMin = secondCollider.aabb3D.GetMin();
	Vector3f secondMax = secondCollider.aabb3D.GetMax();
	secondMin += secondTransform.transform.GetPosition();
	secondMax += secondTransform.transform.GetPosition();

	if (firstMax.x > secondMin.x &&
		firstMin.x < secondMax.x &&
		firstMax.y > secondMin.y &&
		firstMin.y < secondMax.y &&
		firstMax.z > secondMin.z &&
		firstMin.z < secondMax.z)
	{
		return true;
	}

	return false;
}

bool CollisionSystem::LayerCheck(Entity aFirstEntity, Entity aSecondEntity)
{
	eCollisionLayer firstEnum = myWorld->GetComponent<CollisionDataComponent>(aFirstEntity).layer;
	eCollisionLayer secondEnum = myWorld->GetComponent<CollisionDataComponent>(aSecondEntity).layer;

	auto firstLayer = myCollisionLayers.at(firstEnum);
	auto secondLayer = myCollisionLayers.at(secondEnum);

	if (firstLayer.GetBit((int)secondEnum) && secondLayer.GetBit((int)firstEnum))
	{
		return true;
	}
	return false;
}

void CollisionSystem::DrawAllColliders()
{
	auto& dbg = GraphicsEngine::GetInstance()->GetDebugRenderer();

	for (auto& entity : myEntities)
	{
		auto& collider = myWorld->GetComponent<ColliderComponent>(entity);
		auto& collisionData = myWorld->GetComponent<CollisionDataComponent>(entity);
		auto& transform = myWorld->GetComponent<TransformComponent>(entity);

		Vector3f min = collider.aabb3D.GetMin();
		Vector3f max = collider.aabb3D.GetMax();
		min += transform.transform.GetPosition();
		max += transform.transform.GetPosition();

		if (collisionData.isColliding)
			collider.color = { 1.f, 0.f, 0.f };
		else
			collider.color = { 1.f, 1.f, 1.f };

		dbg.DrawLine({ min.x, min.y, min.z }, { min.x, min.y, max.z }, collider.color);
		dbg.DrawLine({ min.x, min.y, max.z }, { max.x, min.y, max.z }, collider.color);
		dbg.DrawLine({ max.x, min.y, max.z }, { max.x, min.y, min.z }, collider.color);
		dbg.DrawLine({ max.x, min.y, min.z }, { min.x, min.y, min.z }, collider.color);
		dbg.DrawLine({ min.x, min.y, min.z }, { min.x, max.y, min.z }, collider.color);
		dbg.DrawLine({ min.x, min.y, max.z }, { min.x, max.y, max.z }, collider.color);
		dbg.DrawLine({ max.x, min.y, max.z }, { max.x, max.y, max.z }, collider.color);
		dbg.DrawLine({ max.x, min.y, min.z }, { max.x, max.y, min.z }, collider.color);
		dbg.DrawLine({ min.x, max.y, min.z }, { min.x, max.y, max.z }, collider.color);
		dbg.DrawLine({ min.x, max.y, max.z }, { max.x, max.y, max.z }, collider.color);
		dbg.DrawLine({ max.x, max.y, max.z }, { max.x, max.y, min.z }, collider.color);
		dbg.DrawLine({ max.x, max.y, min.z }, { min.x, max.y, min.z }, collider.color);
	}
}

void CollisionSystem::SetupCollisionLayers()
{
	Bitmask playerLayer; //Creates the LayerMask
	//Set bit for which to collide with in section below
	playerLayer.SetBit(static_cast<int>(eCollisionLayer::EnemyAttack));
	playerLayer.SetBit(static_cast<int>(eCollisionLayer::Event));
	//playerLayer.SetBit(static_cast<int>(eCollisionLayer::Everything));

	// Add de LayerMask into map with the corresponding CollisionLayer
	myCollisionLayers.insert({ eCollisionLayer::Player, playerLayer });

	Bitmask playerAttackHitboxLayer;
	playerAttackHitboxLayer.SetBit(static_cast<int>(eCollisionLayer::Enemy));
	//playerAttackHitboxLayer.SetBit(static_cast<int>(eCollisionLayer::Everything));
	myCollisionLayers.insert({ eCollisionLayer::PlayerAttack, playerAttackHitboxLayer });

	Bitmask enemyLayer;
	enemyLayer.SetBit(static_cast<int>(eCollisionLayer::PlayerAttack));
	//enemyLayer.SetBit(static_cast<int>(eCollisionLayer::Everything));
	myCollisionLayers.insert({ eCollisionLayer::Enemy, enemyLayer });

	Bitmask enemyAttackHitboxLayer;
	enemyAttackHitboxLayer.SetBit(static_cast<int>(eCollisionLayer::Player));
	//enemyAttackHitboxLayer.SetBit(static_cast<int>(eCollisionLayer::Everything));
	myCollisionLayers.insert({ eCollisionLayer::EnemyAttack, enemyAttackHitboxLayer });


	Bitmask eventLayer;
	eventLayer.SetBit(static_cast<int>(eCollisionLayer::Player));
	//eventLayer.SetBit(static_cast<int>(eCollisionLayer::Everything));
	myCollisionLayers.insert({ eCollisionLayer::Event, eventLayer });


	Bitmask everythingLayer;
	everythingLayer.SetBit(static_cast<int>(eCollisionLayer::Player));
	everythingLayer.SetBit(static_cast<int>(eCollisionLayer::PlayerAttack));
	everythingLayer.SetBit(static_cast<int>(eCollisionLayer::Enemy));
	everythingLayer.SetBit(static_cast<int>(eCollisionLayer::EnemyAttack));
	everythingLayer.SetBit(static_cast<int>(eCollisionLayer::Event));
	myCollisionLayers.insert({ eCollisionLayer::Everything, everythingLayer });

}

Ray<float> CollisionSystem::CreateRayFromMousePosition()
{
	Ray<float> newRay;
	auto* camera = GraphicsEngine::GetInstance()->GetCamera();
	auto viewportDimensions = GraphicsEngine::GetInstance()->DX().GetViewportDimensions();
	auto nearPlaneDimensions = camera->GetNearPlaneDimensions();
	auto farPlaneDimensions = camera->GetFarPlaneDimensions();

	Vector2f mousePosRatio = InputManager::GetInstance()->GetCurrentMousePositionVector2f();
	mousePosRatio.x /= (float)viewportDimensions.x;
	mousePosRatio.y /= (float)viewportDimensions.y;
	mousePosRatio -= { 0.5f, 0.5f };

	Vector3f initialPos = camera->GetTransform().GetPosition();
	initialPos += camera->GetTransform().GetForward().GetNormalized() * camera->GetNearPlane();

	Vector3f xDiff = camera->GetTransform().GetRight() * mousePosRatio.x * nearPlaneDimensions.x * 0.65f;
	Vector3f yDiff = camera->GetTransform().GetUp() * mousePosRatio.y * nearPlaneDimensions.y * 0.56f;

	Vector3f start = initialPos + xDiff + yDiff;

	float xRatio = farPlaneDimensions.x / nearPlaneDimensions.x;
	float yRatio = farPlaneDimensions.y / nearPlaneDimensions.y;

	Vector3f end = initialPos;
	end += xDiff * xRatio;
	end += yDiff * yRatio;
	end += camera->GetTransform().GetForward() * camera->GetFarPlane();

	Vector3 direction = end - start;

	newRay.InitWithOriginAndDirection(start, direction);
	return newRay;
}
