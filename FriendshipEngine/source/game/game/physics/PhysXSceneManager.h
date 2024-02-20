#pragma once

#include <PhysX/PxPhysicsAPI.h>
#include <engine/math/Transform.h>

static const float GRAVITY = -9.82f * 100.0f;
static constexpr float STEP_OFFSET = 50.0f;
static constexpr float HEAD_BONK_OFFSET = 25.0f;
static constexpr float CHARACTER_RADIUS = 50.0f - HEAD_BONK_OFFSET;
static constexpr float CHARACTER_HEIGHT_PRE_ADJUSTMENT = 170.0f;
static constexpr float CHARACTER_HEIGHT = CHARACTER_HEIGHT_PRE_ADJUSTMENT - CHARACTER_RADIUS * 2.0f - HEAD_BONK_OFFSET;
using namespace physx;

namespace physx
{
	class PxScene;
	class PxRigidStatic;
	class PxRigidDynamic;
	class PxController;
	class PxUserControllerHitReport;
}

struct PhysXComponent;

enum CollisionGroups
{
	PLAYER_GROUP = 1 << 0,
	OBJECT_GROUP = 1 << 1,
};

class PhysXSceneManager
{
public:
	PhysXSceneManager();
	~PhysXSceneManager();

	void Init();

	void Update(float dt);
	physx::PxController* CreateCharacterController(Transform& aTransform, physx::PxUserControllerHitReport* aHitReport = nullptr);
	physx::PxRigidDynamic* CreateDynamicCapsule(Transform& aTransform, float aRadius, float aHalfHeight);
	physx::PxRigidDynamic* CreateDynamicSphere(Transform& aTransform, float aRadius);
	physx::PxRigidDynamic* CreateDynamicBox(Transform& aTransform, Vector3f aColliderExtents);
	physx::PxRigidStatic* CreateStatic(Transform& aTransform, Vector3f aColliderExtents);


	physx::PxScene* GetScene() { return myScene; }
private:
	PxDefaultAllocator myAllocator;
	PxDefaultErrorCallback myErrorCallback;
	PxDefaultCpuDispatcher* myDispatcher;

	PxPvd* myPvd;
	PxScene* myScene;
	PxPhysics* myPhysics;
	PxMaterial* myMaterial;
	PxFoundation* myFoundation;
	PxControllerManager* myControllerManager;


	physx::PxController* myController;

#ifdef _DEBUG
	//DebugLine myFinalLine = DebugLine({},{});
#endif // _DEBUG
};
