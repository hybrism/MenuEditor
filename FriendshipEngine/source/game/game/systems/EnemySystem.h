#pragma once
#include "../component/EnemyComponent.h"
#include <physics/PhysXSceneManager.h>

struct TransformComponent;
struct ProjectileComponent;
enum class eEnemyState;

struct Circle
{
	Vector3f center;
	float radius;
};

class EnemySystem : public System<EnemySystem>
{
public:
	EnemySystem(World* aWorld, PhysXSceneManager* aPhysXSceneManager);
	~EnemySystem() override;

	void Update(SceneUpdateContext& aContext) override;
	void AimAtPlayer(TransformComponent& aTransform,const float& aDT,EnemyComponent& aEnemy);
	Vector3f CreateCircleOffsetDir(const float& aLength,EnemyComponent& aEnemy);
	bool isPlayerOverlaping(EnemyComponent& aEnemy);
	bool IsPlayerHit(physx::PxScene* aScene, PxVec3 aRayOrigin, PxVec3 rayDirection, float rayLength);
	void Render() override;
	//void SetEnemyState(eEnemyState aState);

private:
	Vector3f myPlayerPos;
	Vector3f myDirection;
	float myCheckIntervall= 5.0f;
	float myTimer = 0.0f;
	bool myBoolToCheck;
	bool myOverlap;
	DirectX::XMMATRIX myMatrix;
	PhysXSceneManager* myPhysXSceneManager;

	Circle myAimOffsetCircle;

#ifndef _RELEASE
	DebugLine myRandomPoint = DebugLine({ 0,0,0 }, { 0,0,0 });
	DebugLine myCircleRadius = DebugLine({ 0,0,0 }, { 0,0,0 });
	DebugLine myPlayerPosLine = DebugLine({ 0,0,0 }, { 0,0,0 });
	DebugLine myRayCastLine = DebugLine({ 0,0,0 }, { 0,0,0 });
#endif
};


