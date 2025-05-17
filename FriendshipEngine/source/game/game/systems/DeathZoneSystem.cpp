#include "pch.h"
#include "DeathZoneSystem.h"
#include <ecs/World.h>
#include "../component/PlayerComponent.h"
#include "../component/DeathZoneComponent.h"


DeathZoneSystem::DeathZoneSystem(World* aWorld, PhysXSceneManager* aPhysXSceneManager) : System(aWorld), myPhysXSceneManager(aPhysXSceneManager)
{

	ComponentSignature deathzoneSignature;
	deathzoneSignature.set(myWorld->GetComponentSignatureID<DeathZoneComponent>());
	aWorld->SetSystemSignature<DeathZoneSystem>(deathzoneSignature);
}

DeathZoneSystem::~DeathZoneSystem()
{
}

void DeathZoneSystem::Init()
{

}

void DeathZoneSystem::Update(SceneUpdateContext& dt)
{
	UNREFERENCED_PARAMETER(dt);

	for (Entity entity : myEntities)
	{

		DeathZoneComponent& deathzone = myWorld->GetComponent<DeathZoneComponent>(entity);
		Entity playerID = myWorld->GetPlayerEntityID();
		PlayerComponent& playerComp = myWorld->GetComponent<PlayerComponent>(playerID);
		if (IsInsideDeathZone(deathzone,playerComp))
		{
			playerComp.isDead = true;
		}
	
	}
	
}

bool DeathZoneSystem::IsInsideDeathZone(DeathZoneComponent& aDeath, PlayerComponent& aPlayer)
{
	physx::PxOverlapBuffer hit = {};
	physx::PxVec3 halfExtents(aDeath.myScale.x, aDeath.myScale.y, aDeath.myScale.z); // Half extents of the box in each dimension
	physx::PxBoxGeometry shapeBox(halfExtents);

	physx::PxTransform triggerTransform(physx::PxVec3((float)aDeath.overlapPos.x, (float)aDeath.overlapPos.y, (float)aDeath.overlapPos.z));
	myPhysXSceneManager->GetScene()->overlap(shapeBox, triggerTransform, hit, physx::PxQueryFilterData(PxQueryFlag::eANY_HIT | physx::PxQueryFlag::eDYNAMIC));


	if (hit.block.actor != nullptr)
	{
		if (hit.block.actor == aPlayer.controller->getActor())
		{
			return true;
		}
	}
	return false;
}
