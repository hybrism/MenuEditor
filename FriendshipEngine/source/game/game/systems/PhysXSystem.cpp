#include "pch.h"
#include "PhysXSystem.h"
#include <PhysX\PxActor.h>
#include <iostream>
#include <cassert>

#include "../physics/PhysXSceneManager.h"
#include "../component/TransformComponent.h"
#include "../component/PhysXComponent.h"
#include "../component/ColliderComponent.h"
#include <ecs/World.h>

PhysXSystem::PhysXSystem(World* aWorld, PhysXSceneManager* aPhysXSceneManager) : System(aWorld)
{
	myPhysXSceneManager = aPhysXSceneManager;


	ComponentSignature signature;
	signature.set(myWorld->GetComponentSignatureID<TransformComponent>());
	signature.set(myWorld->GetComponentSignatureID<PhysXComponent>());

	aWorld->SetSystemSignature<PhysXSystem>(signature);
}

PhysXSystem::~PhysXSystem()
{
}

void PhysXSystem::Init()
{

}

void PhysXSystem::Update(const float& dt)
{
	myPhysXSceneManager->Update(dt);

	for (auto& entity : myEntities)
	{
		PhysXComponent& physXComp = myWorld->GetComponent<PhysXComponent>(entity);
		TransformComponent& transformComp = myWorld->GetComponent<TransformComponent>(entity);

		//Rot
		QuatToMat(physXComp, transformComp);

		//Pos
		auto pose = physXComp.dynamicPhysX->getGlobalPose();
		Vector3f pos = { pose.p.x, pose.p.y, pose.p.z };
		pos += physXComp.positionOffset;
		transformComp.transform.SetPosition(pos);
	}
}

#include <engine/math/Transform.h>
#include <engine/math/Quaternion.h>
void PhysXSystem::QuatToMat(PhysXComponent& aPhysX, TransformComponent& aTransform)
{
	DirectX::XMMATRIX temp{};

	float qx = aPhysX.dynamicPhysX->getGlobalPose().q.x;
	float qy = aPhysX.dynamicPhysX->getGlobalPose().q.y;
	float qz = aPhysX.dynamicPhysX->getGlobalPose().q.z;
	float qw = aPhysX.dynamicPhysX->getGlobalPose().q.w;

	//Transform t;
	//Quaternionf q = { qx, qy, qz, qw };
	//Matrix3x3f rotationMatrix = q.GetRotationMatrix33();


	temp.r[0].m128_f32[0] = 1.0f - 2.0f * qy * qy - 2.0f * qz * qz;
	temp.r[0].m128_f32[1] = 2.0f * qx * qy - 2.0f * qz * qw;
	temp.r[0].m128_f32[2] = 2.0f * qx * qz + 2.0f * qy * qw;

	temp.r[1].m128_f32[0] = 2.0f * qx * qy + 2.0f * qz * qw;
	temp.r[1].m128_f32[1] = 1.0f - 2.0f * qx * qx - 2.0f * qz * qz;
	temp.r[1].m128_f32[2] = 2.0f * qy * qz - 2.0f * qx * qw;

	temp.r[2].m128_f32[0] = 2.0f * qx * qz - 2.0f * qy * qw;
	temp.r[2].m128_f32[1] = 2.0f * qy * qz + 2.0f * qx * qw;
	temp.r[2].m128_f32[2] = 1.0f - 2.0f * qx * qx - 2.0f * qy * qy;

	//temp.r[0].m128_f32[0] = rotationMatrix(1, 1);
	//temp.r[0].m128_f32[1] = rotationMatrix(1, 2);
	//temp.r[0].m128_f32[2] = rotationMatrix(1, 3);

	//temp.r[1].m128_f32[0] = rotationMatrix(2, 1);
	//temp.r[1].m128_f32[1] = rotationMatrix(2, 2);
	//temp.r[1].m128_f32[2] = rotationMatrix(2, 3);

	//temp.r[2].m128_f32[0] = rotationMatrix(3, 1);
	//temp.r[2].m128_f32[1] = rotationMatrix(3, 2);
	//temp.r[2].m128_f32[2] = rotationMatrix(3, 3);

	temp.r[3].m128_f32[0] = aTransform.transform.GetPosition().x;
	temp.r[3].m128_f32[1] = aTransform.transform.GetPosition().y;
	temp.r[3].m128_f32[2] = aTransform.transform.GetPosition().z;

	aTransform.transform.SetMatrix(temp);
}
