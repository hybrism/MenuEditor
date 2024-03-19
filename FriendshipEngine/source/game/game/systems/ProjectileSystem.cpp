#include "pch.h"
#include "ProjectileSystem.h"
#include <ecs/World.h>

#include "../component/ProjectileComponent.h"
#include "../component/TransformComponent.h"
#include "../component/PhysXComponent.h" 
#include "factory\ProjectileFactory.h"
ProjectileSystem::ProjectileSystem(World* aWorld) : System(aWorld)
{
	

}

void ProjectileSystem::Init()
{

}

void ProjectileSystem::Update(const SceneUpdateContext& dt)
{
	for (Entity entity : myEntities)
	{

		auto& projectileComponent = myWorld->GetComponent<ProjectileComponent>(entity);
		auto& transform = myWorld->GetComponent<TransformComponent>(entity);
		//auto& physX= myWorld->GetComponent<PhysXComponent>(entity);
		transform;

		dt;

		if (projectileComponent.hit)
		{
			auto& physX = myWorld->GetComponent<PhysXComponent>(entity);
			physX.dynamicPhysX->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
			physX.dynamicPhysX->setLinearVelocity(PxVec3{ 0,0,0 });
			physX.dynamicPhysX->clearForce();
			physX.dynamicPhysX->clearTorque();
			projectileComponent.hit = false;
		}
	}
}