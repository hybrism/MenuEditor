#include "pch.h"
#include "ProjectileFactory.h"
#include <assets/AssetDatabase.h>

ProjectileFactory::ProjectileFactory() = default;
ProjectileFactory::~ProjectileFactory() = default;

void ProjectileFactory::Init(World* aWorld, int poolSize, PhysXSceneManager& aPhysXSceneManager)
{
	myWorld = aWorld;
	objectPool.clear();
	myArrowUsed.clear();

	size_t meshID = AssetDatabase::GetMeshIdFromName("sm_arrow");

	for (int i = 0; i < poolSize; ++i)
	{
		Arrow arrow;
		arrow.eID = myWorld->CreateEntity();
		myWorld->AddComponent<ProjectileComponent>(arrow.eID);
		auto& transform = myWorld->AddComponent<TransformComponent>(arrow.eID);
		transform.transform.SetPosition(myInQuiverPos);
		auto& mesh = myWorld->AddComponent<MeshComponent>(arrow.eID);
		mesh.id = meshID;

		//PhysX
		auto& physXComponent = myWorld->AddComponent<PhysXComponent>(arrow.eID);
		Vector3f extents = { 306, 306, 304 };
		physXComponent.dynamicPhysX = aPhysXSceneManager.CreateDynamicBox(myWorld->GetComponent<TransformComponent>(arrow.eID).transform, extents);
		physXComponent.dynamicPhysX->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		physXComponent.dynamicPhysX->userData = (void*)arrow.eID;



		//For filtering, no idea if it works
		//setupFiltering((PxShape*)physXComponent.dynamicPhysX, OBJECT_GROUP,PLAYER_GROUP);

		//The weight of the arrows
		physXComponent.dynamicPhysX->setMass(10.f);

		//Sets the center of mass
		physXComponent.dynamicPhysX->setCMassLocalPose(PxTransform{ PxVec3{0,0,500} });

		//Adjust collider pos
		OffsetCollider(physXComponent.dynamicPhysX, { 0,0,130.f });

#ifdef _DEBUG
		myWorld->AddComponent<MetaDataComponent>(arrow.eID);
#endif

		//	PxShape* shapes[1];
		//	physXComponent.dynamicPhysX->getShapes(shapes, 1);
		////	const PxU32 projectileCollisionMask = CollisionLayer::DEFAULT | CollisionLayer::PLAYER_GROUP;

		//	setupFiltering(shapes[0], CollisionLayer::OBJECT_GROUP, CollisionLayer::DEFAULT);
		//	
		objectPool.emplace_back(arrow);
	}
}

Arrow& ProjectileFactory::acquireArrow()
{
	if (objectPool.empty())
	{
		objectPool.emplace_back(myArrowUsed.front());
		myArrowUsed.pop_front();
	}

	Arrow& obj = objectPool.front();


	myArrowUsed.emplace_back(obj);
	objectPool.pop_front();
	return obj;
}

void ProjectileFactory::releaseObject(Arrow& obj)
{
	auto& transform = myWorld->GetComponent<TransformComponent>(obj.eID);
	transform.transform.SetPosition(myInQuiverPos);
	objectPool.push_back(obj);
}

void ProjectileFactory::ShootArrow(Vector3f aStartPos, Vector3f aDir)
{
	Arrow& arrow = acquireArrow();
	auto& proj = myWorld->GetComponent<ProjectileComponent>(arrow.eID);
	auto& physX = myWorld->GetComponent<PhysXComponent>(arrow.eID);
	auto& transf = myWorld->GetComponent<TransformComponent>(arrow.eID);
	physX.dynamicPhysX->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
	physX.dynamicPhysX->clearForce();
	physX.dynamicPhysX->clearTorque();

	proj.direction = aDir;
	aDir.Normalize();
	proj.position = Vector3f{ aStartPos.x,aStartPos.y,aStartPos.z };
	transf.transform.SetPosition(proj.position);

	Vector3f right = Vector3f{ 0,1,0 }.Cross(aDir) * -1.0f;
	right.Normalize();
	Vector3f up = right.Cross(aDir) * -1.0f;
	up.Normalize();


	//Matrix3x3f rotmatrix;
	DirectX::XMMATRIX rotmatrix(
		right.x, up.x, aDir.x, 0.0f,
		right.y, up.y, aDir.y, 0.0f,
		right.z, up.z, aDir.z, 0.0f,
		aStartPos.x, aStartPos.y, aStartPos.z, 1.0f
	);

	DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationMatrix(rotmatrix);

	physX.dynamicPhysX->setGlobalPose(
		PxTransform(
			PxVec3(proj.position.x, proj.position.y, proj.position.z),
			ConvertXMVECTORToPxQuat(quaternion)
		)
	);


	physX.dynamicPhysX->addForce(PxVec3(aDir.x, aDir.y, aDir.z) * 100000, PxForceMode::eIMPULSE);
}

void ProjectileFactory::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	// Process contact events
	for (PxU32 i = 0; i < nbPairs; i++) {

		const PxContactPair& cp = pairs[i];

		// Collision involves the arrow
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			PxRigidDynamic* arrow = pairHeader.actors[0]->is<PxRigidDynamic>();

			if (arrow != nullptr)
			{
				Entity& entity = (Entity&)arrow->userData;
				auto& projComp = myWorld->GetComponent<ProjectileComponent>(entity);
				projComp.hit = true;

			}
		}
		/*

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			PxRigidStatic* arrow = pairHeader.actors[1]->is<PxRigidStatic>();

			if (arrow != nullptr)
			{

			}
		}*/

	}
}

void ProjectileFactory::OffsetCollider(PxRigidActor* actor, const PxVec3& offset)
{
	const PxU32 numShapes = actor->getNbShapes();
	PxShape* shapes[8];
	actor->getShapes(shapes, numShapes);

	for (PxU32 i = 0; i < numShapes; ++i)
	{
		PxTransform currentLocalPose = shapes[i]->getLocalPose();
		currentLocalPose.p += offset;
		shapes[i]->setLocalPose(currentLocalPose);
	}
}

void ProjectileFactory::setupFiltering(PxShape* shape, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask;  // word1 = ID mask to filter pairs that trigger a contact callback
	shape->setSimulationFilterData(filterData);
}
