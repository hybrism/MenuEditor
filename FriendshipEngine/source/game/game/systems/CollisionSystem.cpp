#include "pch.h"
#include "CollisionSystem.h"
#include <ecs/World.h>
#include <engine/utility/Error.h>
#include <engine/math/Vector3.h>
#include <engine/debug/DebugLine.h>
#include <fmod/fmod.h>
#include "..\component\TransformComponent.h"
#include "..\component\ColliderComponent.h"
#include "..\component\CollisionDataComponent.h"
#include "..\component\PlayerComponent.h"


#include <engine/graphics/Camera.h>
#include <engine/graphics/GraphicsEngine.h>
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

void CollisionSystem::Update(const float& /*dt*/)
{
	return;

	//We dont want to check collision right now, using PhysX instead 
	//In future we will enable this but only for the Trigger type of collision


	//for (auto& entity : myEntities)
	//{
	//	auto& cd = myWorld->GetComponent<CollisionDataComponent>(entity);
	//	cd.isColliding = false;
	//	cd.damageData.targetIDs.clear();

	//}
	//for (auto& firstEntity : myEntities)
	//{
	//	for (auto& secondEntity : myEntities)
	//	{
	//		if (firstEntity == secondEntity)
	//			continue;


	//		if (LayerCheck(firstEntity, secondEntity) == false)
	//			continue;

	//		//So we dont check collision on all entities only the ones which are close enough, -> myDistanceToCheckCollision <-
	//		if (IsCloseEnoughToCheckCollision(firstEntity, secondEntity) == false)
	//			continue;

	//		if (IsColliding(firstEntity, secondEntity))
	//		{
	//			myWorld->GetComponent<CollisionDataComponent>(firstEntity).isColliding = true;
	//			myWorld->GetComponent<CollisionDataComponent>(secondEntity).isColliding = true;
	//		}
	//	}
	//}
	////for (auto& entity : myEntities)
	////{
	////	auto* transform = World::GetComponent<TransformComponent>(entity);
	////	auto* collider = World::GetComponent<ColliderComponent>(entity);
	////	auto* collisionData = World::GetComponent<CollisionDataComponent>(entity);
	////}


	//CheckHoveredEnemies();

	//checkHoverTimer += dt;
	//if (checkHoverTimer > 0.05f)
	//{
	//	CheckHoveredEnemies();
	//	checkHoverTimer = 0;
	//}

	//Cursor;
	//



}

void CollisionSystem::Render()
{
#ifdef _DEBUG
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

	Vector3f firstMin = firstTransform.transform.GetPosition() - firstCollider.extents;
	Vector3f firstMax = firstTransform.transform.GetPosition() + firstCollider.extents;

	Vector3f secondMin = secondTransform.transform.GetPosition() - secondCollider.extents;
	Vector3f secondMax = secondTransform.transform.GetPosition() + secondCollider.extents;


	//AABB CHECK
	if (
		firstMax.x > secondMin.x &&
		firstMin.x < secondMax.x &&
		firstMax.y > secondMin.y &&
		firstMin.y < secondMax.y &&
		firstMax.z > secondMin.z &&
		firstMin.z < secondMax.z
		)
	{
		auto& firstCollisionData = myWorld->GetComponent<CollisionDataComponent>(aFirstEntityIndex);
		auto& secondCollisionData = myWorld->GetComponent<CollisionDataComponent>(aSecondEntityIndex);
		firstCollisionData.damageData.targetIDs.push_back(secondCollisionData.ownerID);
		secondCollisionData.damageData.targetIDs.push_back(firstCollisionData.ownerID);

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
	//for (auto& entity : myEntities)
	//{
	//	auto& collider = myWorld->GetComponent<ColliderComponent>(entity);
	//	if (collider.debugLines.empty())
	//	{
	//		GenerateDebugLines(entity);
	//	}
	//	else
	//	{
	//		UpdateDebugLines(entity);
	//	}
	//}

}

void CollisionSystem::GenerateDebugLines(Entity /*aEntity*/)
{
	//auto& collider = myWorld->GetComponent<ColliderComponent>(aEntity);
	//auto& transform = myWorld->GetComponent<TransformComponent>(aEntity);
	//auto& collisionData = myWorld->GetComponent<CollisionDataComponent>(aEntity);

	////if (collisionData.isColliding) // Regular Collision
	////	collider.color = { 1,1,0,1 };
	////else if (collisionData.type == eCollisionType::Trigger)
	////	collider.color = { 0,1,1,1 };
	////else							// No Collision
	////	collider.color = { 0,1,0,1 };

	//Vector3f min = collider.aabb3D.GetMin();
	//Vector3f max = collider.aabb3D.GetMax();
	//DebugLine* line1 = new DebugLine(transform.transform.GetPosition(), transform.transform.GetPosition() + transform.transform.GetForward() * 100.0f, collider.color);
	////DebugLine* line1 = new DebugLine(Vector3f{ min.x, min.y, min.z }, Vector3f{ min.x, min.y, max.z }, collider.color);
	////DebugLine* line2 = new DebugLine(Vector3f{ min.x, min.y, max.z }, Vector3f{ max.x, min.y, max.z }, collider.color);
	////DebugLine* line3 = new DebugLine(Vector3f{ max.x, min.y, max.z }, Vector3f{ max.x, min.y, min.z }, collider.color);
	////DebugLine* line4 = new DebugLine(Vector3f{ max.x, min.y, min.z }, Vector3f{ min.x, min.y, min.z }, collider.color);
	////DebugLine* line5 = new DebugLine(Vector3f{ min.x, min.y, min.z }, Vector3f{ min.x, max.y, min.z }, collider.color);
	////DebugLine* line6 = new DebugLine(Vector3f{ min.x, min.y, max.z }, Vector3f{ min.x, max.y, max.z }, collider.color);
	////DebugLine* line7 = new DebugLine(Vector3f{ max.x, min.y, max.z }, Vector3f{ max.x, max.y, max.z }, collider.color);
	////DebugLine* line8 = new DebugLine(Vector3f{ max.x, min.y, min.z }, Vector3f{ max.x, max.y, min.z }, collider.color);
	////DebugLine* line9 = new DebugLine(Vector3f{ min.x, max.y, min.z }, Vector3f{ min.x, max.y, max.z }, collider.color);
	////DebugLine* line10 = new DebugLine(Vector3f{ min.x, max.y, max.z }, Vector3f{ max.x, max.y, max.z }, collider.color);
	////DebugLine* line11 = new DebugLine(Vector3f{ max.x, max.y, max.z }, Vector3f{ max.x, max.y, min.z }, collider.color);
	////DebugLine* line12 = new DebugLine(Vector3f{ max.x, max.y, min.z }, Vector3f{ min.x, max.y, min.z }, collider.color);

	//collider.debugLines.push_back(line1);
	////collider.debugLines.push_back(line2);
	////collider.debugLines.push_back(line3);
	////collider.debugLines.push_back(line4);
	////collider.debugLines.push_back(line5);
	////collider.debugLines.push_back(line6);
	////collider.debugLines.push_back(line7);
	////collider.debugLines.push_back(line8);
	////collider.debugLines.push_back(line9);
	////collider.debugLines.push_back(line10);
	////collider.debugLines.push_back(line11);
	////collider.debugLines.push_back(line12);
	//for (size_t i = 0; i < collider.debugLines.size(); i++)
	//{
	//	collider.debugLines[i]->Init();
	//}
}

void CollisionSystem::UpdateDebugLines(Entity /*aEntity*/)
{
	//auto& collider = myWorld->GetComponent<ColliderComponent>(aEntity);
	//auto& collisionData = myWorld->GetComponent<CollisionDataComponent>(aEntity);
	//auto& transform = myWorld->GetComponent<TransformComponent>(aEntity);

	//if (collisionData.isColliding) // Regular Collision
	//	collider.color = { 1,1,0,1 };
	//else if (collisionData.type == eCollisionType::Trigger)
	//	collider.color = { 0,1,1,1 };
	//else							// No Collision
	//	collider.color = { 0,1,0,1 };


	//for (size_t i = 0; i < collider.debugLines.size(); i++)
	//{
	//	auto debugLine = collider.debugLines[i];
	//	debugLine->SetStartPosition(transform.transform.GetPosition());
	//	debugLine->SetEndPosition(transform.transform.GetPosition() + transform.transform.GetRight() * 100.0f);
	//	debugLine->SetColor(collider.color);
	//	debugLine->DrawLine();
	//}

}

void CollisionSystem::DrawSpecificCollider(Entity aEntityID)
{
	aEntityID;
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

void CollisionSystem::CheckHoveredEnemies()
{


}


Ray<float> CollisionSystem::CreateRayFromMousePosition(Vector2<float> aMousePosition)
{
	aMousePosition;
	Ray<float> newRay;
	auto* camera = GraphicsEngine::GetInstance()->GetCamera();
	auto viewportDimensions = GraphicsEngine::GetInstance()->GetViewportDimensions();
	auto nearPlaneDimensions = camera->GetNearPlaneDimensions();
	auto farPlaneDimensions = camera->GetFarPlaneDimensions();

	Vector2f mousePosRatio = InputManager::GetInstance()->GetCurrentMousePositionVector2f();
	//auto mousePosRatio = aMousePosition;
	mousePosRatio.x /= (float)viewportDimensions.x;
	mousePosRatio.y /= (float)viewportDimensions.y;
	mousePosRatio -= { 0.5f, 0.5f };

	Vector3f initialPos = camera->GetPosition();
	initialPos += camera->GetForward().GetNormalized() * camera->GetNearPlane();

	Vector3f xDiff = camera->GetRight() * mousePosRatio.x * nearPlaneDimensions.x * 0.65f;
	Vector3f yDiff = camera->GetUp() * mousePosRatio.y * nearPlaneDimensions.y * 0.56f;

	Vector3f start = initialPos + xDiff + yDiff;

	float xRatio = farPlaneDimensions.x / nearPlaneDimensions.x;
	float yRatio = farPlaneDimensions.y / nearPlaneDimensions.y;

	Vector3f end = initialPos;
	end += xDiff * xRatio;
	end += yDiff * yRatio;
	end += camera->GetForward() * camera->GetFarPlane();

	Vector3 direction = end - start;

	newRay.InitWithOriginAndDirection(start, direction);
	//newRay.InitWith2Points(start, end);
	return newRay;
}
