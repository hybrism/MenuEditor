#pragma once
#include <ecs\World.h>
#include "../component/ProjectileComponent.h"
#include "../component/TransformComponent.h"
#include "../component/MeshComponent.h"
#include "../component/MetaDataComponent.h"
#include "../component/ColliderComponent.h"
#include "../component/PhysXComponent.h"
#include "physics/PhysXSceneManager.h"
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
	Entity myMeshID = 0;
	Entity myOriginalID;
	World* myWorld = nullptr;
	Vector3f myInQuiverPos = { 0,-50000.f,0 };

public:
	ProjectileFactory()
	{
	};
	~ProjectileFactory() = default;

	static	ProjectileFactory& GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new ProjectileFactory;
		}
		return *myInstance;
	};

	void SetMeshID(Entity aMeshID, World* aWorld)
	{
		myWorld = aWorld;
		auto& mesh = myWorld->GetComponent<MeshComponent>(aMeshID);
		myMeshID = mesh.id;
		myOriginalID = aMeshID;
	};

	void Init(int poolSize, PhysXSceneManager& aPhysXSceneManager)
	{
		if (myMeshID == 0)
		{
			return;
		}

		auto& refCollider = myWorld->GetComponent<ColliderComponent>(myOriginalID);

		for (int i = 0; i < poolSize; ++i)
		{
			Arrow arrow;
			arrow.eID = myWorld->CreateEntity();
			myWorld->AddComponent<ProjectileComponent>(arrow.eID);
			auto& transform = myWorld->AddComponent<TransformComponent>(arrow.eID);
			transform.transform.SetPosition(myInQuiverPos);
			auto& mesh = myWorld->AddComponent<MeshComponent>(arrow.eID);
			mesh.id = myMeshID;

			//PhysX
			auto& physXComponent = myWorld->AddComponent<PhysXComponent>(arrow.eID);
			physXComponent.dynamicPhysX = aPhysXSceneManager.CreateDynamicBox(myWorld->GetComponent<TransformComponent>(arrow.eID).transform, refCollider.extents * 100.f);
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
			objectPool.emplace_back(arrow);
		}
	};

	Arrow& acquireArrow()
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


	// Release an object back to the pool
	void releaseObject(Arrow& obj)
	{
		auto& transform = myWorld->GetComponent<TransformComponent>(obj.eID);
		auto& proj = myWorld->GetComponent<ProjectileComponent>(obj.eID);
		proj.fire = false;
		transform.transform.SetPosition(myInQuiverPos);
		objectPool.push_back(obj);
	}

	void ShootArrow(Vector3f aStartPos, Vector3f aDir)
	{
		Arrow& arrow = acquireArrow();
		auto& proj = myWorld->GetComponent<ProjectileComponent>(arrow.eID);
		auto& physX = myWorld->GetComponent<PhysXComponent>(arrow.eID);
		physX.dynamicPhysX->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
		physX.dynamicPhysX->clearForce();
		physX.dynamicPhysX->clearTorque();

		proj.fire = true;
		proj.direction = aDir;
		aDir.Normalize();
		proj.position = Vector3f{ aStartPos.x,aStartPos.y,aStartPos.z };

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


		physX.dynamicPhysX->addForce(PxVec3(aDir.x, aDir.y, aDir.z) * 100000 * 0.5f, PxForceMode::eIMPULSE);
	};

	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override
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

	PxQuat ConvertXMVECTORToPxQuat(const DirectX::XMVECTOR& xmQuat)
	{
		return PxQuat(xmQuat.m128_f32[0], xmQuat.m128_f32[1], xmQuat.m128_f32[2], xmQuat.m128_f32[3]);
	}

	void OffsetCollider(PxRigidActor* actor, const PxVec3& offset)
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

	void setupFiltering(PxShape* shape, PxU32 filterGroup, PxU32 filterMask)
	{
		PxFilterData filterData;
		filterData.word0 = filterGroup; // word0 = own ID
		filterData.word1 = filterMask;  // word1 = ID mask to filter pairs that trigger a contact callback
		shape->setSimulationFilterData(filterData);
	}

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