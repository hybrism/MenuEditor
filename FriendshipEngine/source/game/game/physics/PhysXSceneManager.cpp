#include "pch.h"
#include "PhysXSceneManager.h"
#include <PhysX/PxPhysicsAPI.h>
#include <PhysX\PxActor.h>

#include "../game/component/PhysXComponent.h"
#include "../engine/engine/debug/DebugLine.h"
#include "factory\ProjectileFactory.h"

//#define PX_PHYSICS_VERSION 0x304000 // TODO: Move to precompiled header

#include <engine\Defines.h>

#define CPU_DISPATCHER_THREADS 2
#define USE_VISUAL_DEBUGGER 0




physx::PxFilterFlags CustomFilterShader(physx::PxFilterObjectAttributes /*attribute0*/, physx::PxFilterData /*filterData0*/, physx::PxFilterObjectAttributes /*attribute1*/, physx::PxFilterData /*filterData1*/, physx::PxPairFlags& pairFlags, const void* /*constantBlock*/, physx::PxU32 /*constantBlockSize*/);

PhysXSceneManager::PhysXSceneManager() :
	myAllocator(),
	myErrorCallback(),
	myDispatcher(nullptr),
	myPvd(nullptr),
	myScene(nullptr),
	myPhysics(nullptr),
	myMaterial(nullptr),
	myFoundation(nullptr),
	myControllerManager(nullptr)
{
	myFoundation = PxCreateFoundation(PX_PHYSICS_FOUNDATION_VERSION, myAllocator, myErrorCallback);

	assert(myFoundation != nullptr && "Failed to create PxFoundation!");

#ifndef _RELEASE
#ifdef USE_VISUAL_DEBUGGER
	// Visual Debugger
	myPvd = PxCreatePvd(*myFoundation);
	myPvd->connect(*PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10), PxPvdInstrumentationFlag::eALL);
#endif
#endif

	myPhysics = PxCreatePhysics(PX_PHYSICS_FOUNDATION_VERSION, *myFoundation, PxTolerancesScale(), true, myPvd);
	myDispatcher = PxDefaultCpuDispatcherCreate(CPU_DISPATCHER_THREADS);
	myMaterial = myPhysics->createMaterial(0.5f, 0.5f, 0.25f);
}

PhysXSceneManager::~PhysXSceneManager()
{

}

void PhysXSceneManager::Init()
{
	if (myScene)
	{
		myControllerManager->release();
		myControllerManager = nullptr;
		myScene->release();
		myScene = nullptr;
	}

	{
		PxSceneDesc sceneDesc(myPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, GRAVITY, 0.0f);
		sceneDesc.cpuDispatcher = myDispatcher;
		sceneDesc.filterShader = CustomFilterShader;// ÄNDRA DETTA FÖR ATT ENABLEA EVENS FRÅN DYNAMICS
		sceneDesc.simulationEventCallback = &ProjectileFactory::GetInstance();
		myScene = myPhysics->createScene(sceneDesc);
	}

	myControllerManager = PxCreateControllerManager(*myScene);

#ifndef _RELEASE
#ifdef USE_VISUAL_DEBUGGER
	// Visual Debugger
	PxPvdSceneClient* pvdClient = myScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		pvdClient->updateCamera("test", PxVec3(0, 10, 10), PxVec3(0, 1, 0), PxVec3(0, 0, 0));
	}
#endif
#endif

	//plane
	PxRigidStatic* groundPlane = PxCreatePlane(*myPhysics, PxPlane(0, 1, 0, 0), *myMaterial);
	//auto transform = groundPlane->getGlobalPose();
	//transform.p = PxVec3(0, -100, 0);
	//groundPlane->setGlobalPose(transform);
	myScene->addActor(*groundPlane);
	myScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	myScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_AABBS, 2.0f);
}

static float currentGrav = 0.0f;
void PhysXSceneManager::Update(float dt)
{
	//controller->getUserData().
	//myController->move(PxVec3(1.0f, 1.0f, 0.0f), 0.0f, 1.0f / 60.0f, PxControllerFilters());
	/*myController->getActor*/
	myScene->simulate(dt);
	myScene->fetchResults(true);


//	//Debug
//#ifndef _RELEASE
//	const PxRenderBuffer& rb = myScene->getRenderBuffer();
//	for (PxU32 i = 0; i < rb.getNbLines(); i++)
//	{
//		const PxDebugLine& line = rb.getLines()[i];
//
//		Vector3f firstPointToRender = { line.pos0.x,line.pos0.y,line.pos0.z };
//		Vector3f secondPointToRender = { line.pos1.x,line.pos1.y,line.pos1.z };
//
//		myFinalLine = DebugLine(firstPointToRender, secondPointToRender);
//		myFinalLine.SetColor({1,1,0,1});
//		myFinalLine.DrawLine();
//	}
//#endif // _DEBUG

}

#include <engine/math/Math.h>

void setupFiltering(PxShape* shape, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask;  // word1 = ID mask to filter pairs that trigger a contact callback
	shape->setSimulationFilterData(filterData);
}
physx::PxController* PhysXSceneManager::CreateCharacterController(Transform& aTransform, physx::PxUserControllerHitReport* aHitReport)
{
	PxCapsuleControllerDesc desc;
	desc.clientID = 0;
	desc.behaviorCallback = nullptr;
	desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	desc.contactOffset = 0.001f;
	desc.density = 10.0f;
	desc.radius = CHARACTER_RADIUS;
	desc.height = CHARACTER_HEIGHT;
	desc.invisibleWallHeight = 0.0f;
	desc.material = myMaterial;
	desc.maxJumpHeight = 0.0f;
	desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;


	//unused
	//PxSetGroupCollisionFlag(CollisionGroups::PLAYER_GROUP, CollisionGroups::OBJECT_GROUP,false);
	{
		auto position = aTransform.GetPosition();
		desc.position = physx::PxExtendedVec3(position.x, position.y + CHARACTER_HEIGHT_PRE_ADJUSTMENT / 2.0f, position.z);
	}

	desc.reportCallback = aHitReport;
	desc.scaleCoeff = 1.0f;
	desc.slopeLimit = cos(35.0f * Deg2Rad);
	desc.stepOffset = STEP_OFFSET;
	desc.upDirection = physx::PxVec3(0.0f, 1.0f, 0.0f);
	desc.userData = nullptr;
	desc.volumeGrowth = 1.5f;

	myController = myControllerManager->createController(desc);
	myController->getActor();



	//physx::PxShape** shapes = new physx::PxShape * [myController->getActor()->getNbShapes()];
	//physx::PxU32 bufferSize = sizeof(physx::PxShape) * myController->getActor()->getNbShapes();
	//myController->getActor()->getShapes(shapes, bufferSize, 0);

	//for (physx::PxU32 shapeIndex = 0; shapeIndex < myController->getActor()->getNbShapes(); shapeIndex++)
	//{

	//	if (shapeIndex == 0)
	//	{
	//		physx::PxFilterData filterData;
	//		filterData.word0 = CollisionLayer::PLAYER_GROUP;
	//		filterData.word1 = CollisionLayer::DEFAULT;
	//		shapes[0]->setQueryFilterData(filterData);
	//		shapes[0]->setSimulationFilterData(filterData);
	//	}
	//}

	//delete[] shapes;

	//PxShape* playerCCTShapes[1];
	//myController->getActor()->getShapes(playerCCTShapes, 1);
	//setupFiltering(playerCCTShapes[0], CollisionLayer::PLAYER_GROUP , CollisionLayer::DEFAULT);

	return myController;
}

physx::PxRigidDynamic* PhysXSceneManager::CreateDynamicCapsule(
	Transform& aTransform,
	float aRadius,
	float aHalfHeight
)

{
	float divider = 200;

	Vector3f position = aTransform.GetPosition();
	aRadius /= divider;

	physx::PxRigidDynamic* result = PxCreateDynamic(
		*myPhysics,
		PxTransform(PxVec3(position.x, position.y, position.z)),
		PxCapsuleGeometry(aRadius, aHalfHeight),
		*myMaterial,
		10.f
	);

	result->setAngularDamping(0.5f);
	result->setLinearVelocity(PxVec3(0, 0, 0));
	myScene->addActor(*result);

	return result;
}

physx::PxRigidDynamic* PhysXSceneManager::CreateDynamicSphere(Transform& aTransform, float aRadius)
{
	float divider = 200;

	Vector3f position = aTransform.GetPosition();
	aRadius /= divider;

	physx::PxRigidDynamic* result = PxCreateDynamic(
		*myPhysics,
		PxTransform(PxVec3(position.x, position.y, position.z)),
		PxSphereGeometry(aRadius),
		*myMaterial,
		10.f
	);

	result->setAngularDamping(0.5f);
	result->setLinearVelocity(PxVec3(0, 0, 0));
	myScene->addActor(*result);

	return result;
}

physx::PxRigidDynamic* PhysXSceneManager::CreateDynamicBox(Transform& aTransform, Vector3f aColliderExtents)
{
	float divider = 200;

	Vector3f position = aTransform.GetPosition();
	aColliderExtents.x /= divider;
	aColliderExtents.y /= divider;
	aColliderExtents.z /= divider;

	physx::PxRigidDynamic* result = PxCreateDynamic(
		*myPhysics,
		PxTransform(PxVec3(position.x, position.y, position.z)),
		PxBoxGeometry(aColliderExtents.x, aColliderExtents.y, aColliderExtents.z),
		*myMaterial,
		10.f
	);

	result->setAngularDamping(0.5f);
	result->setLinearVelocity(PxVec3(0, 0, 0));
	myScene->addActor(*result);

	return result;
}

physx::PxRigidStatic* PhysXSceneManager::CreateStatic(Transform& aTransform, Vector3f aColliderExtents)
{
	Vector3f position = aTransform.GetPosition();
	Vector3f eulerRot = aTransform.GetEulerRotation();

	//Transform modelS = aTransform;
	Transform modelT{};
	modelT.SetPosition(aColliderExtents);

	Transform finalTransformPOS = modelT.GetMatrix() * aTransform.GetMatrix();

	//Transform modelT = aTransform;

	//r.SetEulerAngles({ 0,0,0 });

	//Transform finalTransform = r.GetMatrix() * finalTransformPOS.GetMatrix();
	//Transform superFinal = finalTransform.GetMatrix() * finalTransformPOS.GetMatrix();

	auto q = DirectX::XMQuaternionRotationMatrix(aTransform.GetMatrix());

	Vector3f finalPos = finalTransformPOS.GetPosition();
	PxQuat staticQuat(q.m128_f32[0], q.m128_f32[1], q.m128_f32[2], q.m128_f32[3]);
	PxVec3 staticVector3f(finalPos.x, finalPos.y, finalPos.z);

	PxTransform staticTransform(staticVector3f, staticQuat);

	PxBoxGeometry staticBox(aColliderExtents.x, aColliderExtents.y, aColliderExtents.z);

	//PxRigidStatic* staticBody = PxCreateStatic(*myPhysics,
	//	PxTransform(PxVec3(position.x + aColliderExtents.x, position.y + aColliderExtents.y, position.z + aColliderExtents.z),
	//		PxQuat(q.m128_f32[0], q.m128_f32[1], q.m128_f32[2], q.m128_f32[3])),
	//	PxBoxGeometry(aColliderExtents.x, aColliderExtents.y, aColliderExtents.z),
	//	*myMaterial
	//);
	PxRigidStatic* staticBody = PxCreateStatic(
		*myPhysics,
		staticTransform,
		staticBox,
		*myMaterial
	);
	//auto LASDstaticTransform = staticBody->getGlobalPose();
	//PxVec3 newPos = { staticTransform.p.x + aColliderExtents.x, staticTransform.p.y + aColliderExtents.y, staticTransform.p.z + aColliderExtents.z };
	//staticTransform.p = newPos;
	//staticBody->setGlobalPose(staticTransform);
	//staticQuat.
	myScene->addActor(*staticBody);

	return staticBody;
}

physx::PxFilterFlags CustomFilterShader(physx::PxFilterObjectAttributes attribute0, physx::PxFilterData /*filterData0*/, physx::PxFilterObjectAttributes attribute1, physx::PxFilterData /*filterData1*/, physx::PxPairFlags& pairFlags, const void* /*constantBlock*/, physx::PxU32 /*constantBlockSize*/)
{
	// https://nvidia-omniverse.github.io/PhysX/physx/5.1.0/docs/RigidBodyCollision.html#collisionfiltering

	//Let Triggers through if set to: physx::PxPairFlag::eTRIGGER_DEFAULT
	if (physx::PxFilterObjectIsTrigger(attribute0) || physx::PxFilterObjectIsTrigger(attribute1))
	{
	    pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
	    return physx::PxFilterFlag::eSUPPRESS;

	}

	// Enable collisions between everything except the player and objects
	if ((attribute0 & PLAYER_GROUP) && (attribute1 & OBJECT_GROUP))
	{
		pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	// Generate contacts for all that were not filtered above
	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
	pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;



	// The filterData is data set by the user
	// The words can be layers or IDs that can be used to compare with other objects.

	// Trigger the contact callback for pairs (A, B) where
	// the filtermask of A contains the ID of B and vice versa.
	//if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	//{
	//    pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
	//}

	return physx::PxFilterFlag::eDEFAULT;
}

