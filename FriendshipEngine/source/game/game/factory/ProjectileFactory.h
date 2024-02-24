#pragma once
#include <ecs\World.h>
#include "../component/ProjectileComponent.h"
#include "../component/TransformComponent.h"
#include "../component/MeshComponent.h"
#include "../component/MetaDataComponent.h"
#include "../component/ColliderComponent.h"
#include "../component/PhysXComponent.h"
#include "physics/PhysXSceneManager.h"
#include <engine\Defines.h>
#include <DirectXMath.h>
#include <iostream>
class Arrow
{
public:
	Arrow() = default;
	Entity eID;
private:
};

class ProjectileFactory : public PxSimulationEventCallback
{
private:
	std::deque<Arrow> objectPool;
	std::deque<Arrow> myArrowUsed;

	static inline ProjectileFactory* myInstance = nullptr;
	Entity myOriginalID;
	World* myWorld = nullptr;
	Vector3f myInQuiverPos = { 0,-50000.f,0 };

public:
	ProjectileFactory();
	~ProjectileFactory();

	static	ProjectileFactory& GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new ProjectileFactory;
		}
		return *myInstance;
	};

	void Init(World* aWorld, int poolSize, PhysXSceneManager& aPhysXSceneManager);

	Arrow& acquireArrow();


	// Release an object back to the pool
	void releaseObject(Arrow& obj);

	void ShootArrow(Vector3f aStartPos, Vector3f aDir);

	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;

	PxQuat ConvertXMVECTORToPxQuat(const DirectX::XMVECTOR& xmQuat)
	{
		return PxQuat(xmQuat.m128_f32[0], xmQuat.m128_f32[1], xmQuat.m128_f32[2], xmQuat.m128_f32[3]);
	}

	void OffsetCollider(PxRigidActor* actor, const PxVec3& offset);

	void setupFiltering(PxShape* shape, PxU32 filterGroup, PxU32 filterMask);

	void onConstraintBreak(PxConstraintInfo* /*constraints*/, PxU32 /*count*/) override
	{
	}
	void onWake(PxActor** /*actors*/, PxU32 /*count*/) override
	{
	}
	void onSleep(PxActor** /*actors*/, PxU32 /*count*/) override
	{
	}
	void onTrigger(PxTriggerPair* /*pairs*/, PxU32 /*count*/) override
	{


	}
	void onAdvance(const PxRigidBody* const* /*bodyBuffer*/, const PxTransform* /*poseBuffer*/, const PxU32 /*count*/) override
	{
	}
};