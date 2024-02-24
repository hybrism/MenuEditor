#include "pch.h"
#include "NpcSystem.h"
#include <ecs/World.h>
#include "../component/NpcComponent.h"
#include <engine\graphics\Animation\AnimationController.h>

#include "../component/TransformComponent.h"
#include <iostream>
NpcSystem::NpcSystem(World* aWorld) : System(aWorld)
{
}

void NpcSystem::Init()
{
	for (Entity entity : myEntities)
	{

		auto& npcComponent = myWorld->GetComponent<NpcComponent>(entity);
		auto& transform = myWorld->GetComponent<TransformComponent>(entity);

		npcComponent.StartPos = transform.transform.GetPosition();
		npcComponent.lerpingPos = npcComponent.StartPos;

	}

}

void NpcSystem::Update(const float& dt)
{


	for (Entity entity : myEntities)
	{

		auto& npcComponent = myWorld->GetComponent<NpcComponent>(entity);
		auto& transform = myWorld->GetComponent<TransformComponent>(entity);

		AnimationDataParameter parameter;
		parameter.i = static_cast<int>(npcComponent.myNpcState);
		
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
		Vector3f dir = aNpc.walkToPos - aNpc.lerpingPos;
		dir.Normalize();
		Vector3f movement = dir * 250.f * dt;
		aNpc.lerpingPos += movement;

		if ((aNpc.walkToPos - aNpc.lerpingPos).LengthSqr() <= (movement.LengthSqr()))
		{
			//	aTransform.transform.SetEulerAngles();
			aNpc.lerpingPos = aNpc.walkToPos;

			Vector3f temp = aNpc.walkToPos;
			aNpc.walkToPos = aNpc.StartPos;
			aNpc.StartPos = temp;
		}

		aTransform.transform.SetPosition(aNpc.lerpingPos);

		if (dir != Vector3f(0, 0, 0))
		{
			float yaw = atan2f(-dir.x, dir.z) * 180.0f / M_FRIEND_PI;

			aTransform.transform.SetEulerAngles(Vector3f(0, yaw, 0));

		}

		else if (aNpc.GroupID == 2 && aNpc.IsActive == true)
		{



		}


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
