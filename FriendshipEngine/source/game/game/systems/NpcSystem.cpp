#include "pch.h"
#include "NpcSystem.h"
#include <ecs/World.h>
#include "../component/NpcComponent.h"
#include "../component/TransformComponent.h"
#include <iostream>
NpcSystem::NpcSystem(World* aWorld) : System(aWorld)
{
}

void NpcSystem::Init()
{
}

void NpcSystem::Update(const float& dt)
{
	UNREFERENCED_PARAMETER(dt);


	for (Entity entity : myEntities)
	{

		auto& npcComponent = myWorld->GetComponent<NpcComponent>(entity);
		auto& transform = myWorld->GetComponent<TransformComponent>(entity);

		
		FollowPath(dt, npcComponent, transform);
	}

	
}

void NpcSystem::FollowPath(const float& dt, NpcComponent& aNpc, TransformComponent& aTransform)
{
	if (GetAsyncKeyState('Z'))
	{
		ActivateGroup(1);
	}
	
	if (aNpc.GroupID == 1 && aNpc.IsActive == true)
	{
		Vector3f myPos = aTransform.transform.GetPosition();
		aTransform.transform.SetPosition({ myPos.x + 100 * dt,myPos.y,myPos.z } );

	}

	else if (aNpc.GroupID == 2 && aNpc.IsActive == true)
	{



	}


}

void NpcSystem::ActivateGroup(int aGroupID)
{
	for (Entity entity : myEntities)
	{
		auto& npcComponent = myWorld->GetComponent<NpcComponent>(entity);
		if (npcComponent.GroupID == aGroupID)
		{
			npcComponent.IsActive = true;
		}
	}
}
