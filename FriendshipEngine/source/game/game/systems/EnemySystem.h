#pragma once
#include "../component/EnemyComponent.h"

struct TransformComponent;
struct ProjectileComponent;
enum class eEnemyState;


struct Circle
{
	Vector3f center;
	float radius;
};

class EnemySystem : public System
{
public:
	EnemySystem(World* aWorld);
	~EnemySystem() override;

	void Update(const float& aDT) override;
	void AimAtPlayer(TransformComponent& aTransform,const float& aDT);
	Vector3f CreateCircleOffsetDir(const float& aLength);
//	bool IsPlayerHit(PxController* aController, PxScene* aScene, PxVec3 aRayOrigin, PxVec3 rayDirection, float rayLength);
	void Render() override;
	//void SetEnemyState(eEnemyState aState);

private:
	Vector3f myPos;
	Vector3f myPlayerPos;
	Vector3f myDirection;

	DirectX::XMMATRIX myMatrix;

	Circle myAimOffsetCircle;

#ifdef _DEBUG
	DebugLine myRandomPoint = DebugLine({ 0,0,0 }, { 0,0,0 });
	DebugLine myCircleRadius = DebugLine({ 0,0,0 }, { 0,0,0 });
	DebugLine myPlayerPosLine = DebugLine({ 0,0,0 }, { 0,0,0 });
#endif
};


